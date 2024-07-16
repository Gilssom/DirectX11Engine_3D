#include "pch.h"
#include "CRenderManager.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CAssetManager.h"

#include "CCamera.h"
#include "CLight2D.h"
#include "CLight3D.h"

#include "CLevelManager.h"

#include "CMRT.h"

#include "CFontManager.h"

CRenderManager::CRenderManager()
	 : m_EditorCam(nullptr)
	 , m_Light2DBuffer(nullptr)
	 , m_Light3DBuffer(nullptr)
	 , m_MRT{}
	 , m_BoundingBox(true)
	 , m_TotalDrawCall(0)
{
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light3DBuffer = new CStructuredBuffer;

	Render_Func = &CRenderManager::Render_Play;
}

CRenderManager::~CRenderManager()
{
	delete m_Light2DBuffer;
	delete m_Light3DBuffer;

	Safe_Del_Array(m_MRT);
}

void CRenderManager::CopyRenderTarget()
{
	Ptr<CTexture> pRenderTargetTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_RenderTargetCopyTex->GetTex2D().Get(), pRenderTargetTex->GetTex2D().Get());
}

void CRenderManager::Tick()
{
}

void CRenderManager::Render()
{
	// Level 이 존재하지 않으면 Rendering X
	if (!CLevelManager::GetInst()->GetCurrentLevel())
		return;

	m_TotalDrawCall = 0;

	// MRT Clear
	ClearMRT();

	// 렌더링에 필요한 데이터 바인딩
	DataBinding();

	// 함수 포인터로 원하는 함수를 호출 시킬 수 있도록 (Render Play or Editor)
	(this->*Render_Func)();

	// 데이터 클리어
	DataClear();
}

void CRenderManager::Render_Play()
{
	// Camera Draw in Render Target
	for (size_t i = 0; i < m_vecCam.size(); i++)
	{
		if (m_vecCam[i] == nullptr)
			continue;

		m_vecCam[i]->Render();
	}
}

void CRenderManager::Render_Editor()
{
	if (m_EditorCam == nullptr)
		return;

	g_Trans.matView		= m_EditorCam->GetViewMat();
	g_Trans.matViewInv	= m_EditorCam->GetViewInvMat();
	g_Trans.matProj		= m_EditorCam->GetProjMat();
	g_Trans.matProjInv	= m_EditorCam->GetProjInvMat();

	// Shader Domain 에 따른 물체의 분류 작업
	m_EditorCam->SortObject();

	// Deferred MRT 로 변경
	m_MRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();

	// Deferred 물체 Rendering
	m_EditorCam->Render_deferred();
	
	// Decal MRT 로 변경
	m_MRT[(UINT)MRT_TYPE::DECAL]->OMSet();

	// Decal 물체 Rendering
	m_EditorCam->Render_decal();

	// Light MRT 로 변경
	m_MRT[(UINT)MRT_TYPE::LIGHT]->OMSet();

	// Light Rendering
	for (size_t i = 0; i < m_vecLight3D.size(); i++)
	{
		m_vecLight3D[i]->Lighting();
		AddDrawCall();
	}
	
	// Color 정보와 Light 정보를 병합
	// 목적지는 SwapChain
	// Deferred 오브젝트들의 색상이 기록된 Color Target + 빛이 기록되어 있는 Diffuse, Specular, Emissive

	// Swap Chain MRT 로 변경
	m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();

	Ptr<CMesh> pRectMesh = CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh");
	Ptr<CMaterial> pMergeMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"MergeMaterial");

	// 특정 Render Target 을 출력할 경우
	if (m_OutputTarget)
	{
		pMergeMaterial->SetTexParam(TEX_0, nullptr);
		pMergeMaterial->SetTexParam(TEX_1, nullptr);
		pMergeMaterial->SetTexParam(TEX_2, nullptr);
		pMergeMaterial->SetTexParam(TEX_3, nullptr);
		pMergeMaterial->SetTexParam(TEX_4, nullptr);
		pMergeMaterial->SetTexParam(TEX_5, m_OutputTargetTexture);

		pMergeMaterial->SetScalarParam(INT_0, 1);
	}
	else
	{
		pMergeMaterial->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"));
		pMergeMaterial->SetTexParam(TEX_1, CAssetManager::GetInst()->FindAsset<CTexture>(L"DiffuseTargetTex"));
		pMergeMaterial->SetTexParam(TEX_2, CAssetManager::GetInst()->FindAsset<CTexture>(L"SpecularTargetTex"));
		pMergeMaterial->SetTexParam(TEX_3, CAssetManager::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"));
		pMergeMaterial->SetTexParam(TEX_4, CAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
		pMergeMaterial->SetTexParam(TEX_5, m_OutputTargetTexture);

		pMergeMaterial->SetScalarParam(INT_0, 0);
	}

	pMergeMaterial->Binding();
	pRectMesh->Render();
	AddDrawCall();

	// Forward Rendering 진행
	m_EditorCam->Render_opaque();
	m_EditorCam->Render_masked();
	m_EditorCam->Render_transparent();
	m_EditorCam->Render_particle();

	// Post Process 후처리 과정 진행
	m_EditorCam->Render_postprocess();
}

void CRenderManager::Render_DrawCall()
{
	wchar_t szDrawCall[255] = {};

	swprintf_s(szDrawCall, L"Total DrawCall : %d", m_TotalDrawCall);

	CFontManager::GetInst()->DrawFont(szDrawCall, 10.f, 30.f, 20, FONT_RGBA(255, 30, 30, 255));
}

void CRenderManager::DataBinding()
{
	// Global Data Binding
	g_GlobalData.Light2DCount = (int)m_vecLight2D.size();
	g_GlobalData.Light3DCount = (int)m_vecLight3D.size();

	static CConstBuffer* GlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	GlobalBuffer->SetData(&g_GlobalData);
	GlobalBuffer->Binding();
	GlobalBuffer->Binding_CS();



	// =========
	// Light 2D
	// =========
	
	// Light 구조화 버퍼로 Binding
	// 크기가 더 커지면 새롭게 Create ( 공간이 부족한 일이 없어짐 )
	// 광원 개수보다 구조화 버퍼 요소크기가 더 작으면 확장.
	if (m_Light2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		m_Light2DBuffer->Create(sizeof(tLightInfo), (UINT)m_vecLight2D.size(), SB_TYPE::SRV_ONLY, true);
	}

	// m_vecLight2D 에 모인 광원의 정보를 구조화 버퍼로 전달
	static vector<tLightInfo> vecLightInfo;
	vecLightInfo.clear();

	for (size_t i = 0; i < m_vecLight2D.size(); i++)
	{
		vecLightInfo.push_back(m_vecLight2D[i]->GetLightInfo());
	}

	// 광원이 Level 에 1개 이상이 있다면
	if (!vecLightInfo.empty())
	{
		m_Light2DBuffer->SetData(vecLightInfo.data(), m_vecLight2D.size());
		m_Light2DBuffer->Binding(15);
	}



	// =========
	// Light 3D
	// =========
	
	// Light 구조화 버퍼로 Binding
	// 크기가 더 커지면 새롭게 Create ( 공간이 부족한 일이 없어짐 )
	// 광원 개수보다 구조화 버퍼 요소크기가 더 작으면 확장.
	if (m_Light3DBuffer->GetElementCount() < m_vecLight3D.size())
	{
		m_Light3DBuffer->Create(sizeof(tLightInfo), (UINT)m_vecLight3D.size(), SB_TYPE::SRV_ONLY, true);
	}

	// m_vecLight3D 에 모인 광원의 정보를 구조화 버퍼로 전달
	vecLightInfo.clear();

	for (size_t i = 0; i < m_vecLight3D.size(); i++)
	{
		vecLightInfo.push_back(m_vecLight3D[i]->GetLightInfo());
	}

	// 광원이 Level 에 1개 이상이 있다면
	if (!vecLightInfo.empty())
	{
		m_Light3DBuffer->SetData(vecLightInfo.data(), m_vecLight3D.size());
		m_Light3DBuffer->Binding(16);
	}
}

void CRenderManager::DataClear()
{
	m_vecLight2D.clear();
	m_vecLight3D.clear();
}

void CRenderManager::RegisterCamera(CCamera* newCamera, int priority)
{
	// 카메라 등록 | Vector Size 는 필요에 따라 계속 늘리기
	if (m_vecCam.size() <= priority)
	{
		m_vecCam.resize(priority + 1);
	}

	// 이미 해당 자리에 다른 Camera 가 있고, 해당 Camera 와 자기와 다르다면 assert
	if (m_vecCam[priority] && m_vecCam[priority] != newCamera)
		assert(nullptr);

	m_vecCam[priority] = newCamera;
}