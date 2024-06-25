#include "pch.h"
#include "CRenderManager.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CAssetManager.h"

#include "CCamera.h"
#include "CLight2D.h"

#include "CLevelManager.h"

CRenderManager::CRenderManager()
	 : m_EditorCam(nullptr)
	 , m_Light2DBuffer(nullptr)
{
	m_Light2DBuffer = new CStructuredBuffer;

	Render_Func = &CRenderManager::Render_Play;
}

CRenderManager::~CRenderManager()
{
	delete m_Light2DBuffer;
}

void CRenderManager::CopyRenderTarget()
{
	Ptr<CTexture> pRenderTargetTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_RenderTargetCopyTex->GetTex2D().Get(), pRenderTargetTex->GetTex2D().Get());
}

void CRenderManager::Init()
{
	// RenderTarget 을 복사받을 수 있는 텍스쳐를 생성
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();
	m_RenderTargetCopyTex = CAssetManager::GetInst()->CreateTexture(L"RenderTargetCopyTex"
							, (UINT)vRenderResol.x, (UINT)vRenderResol.y
							, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);
}

void CRenderManager::Tick()
{
}

void CRenderManager::Render()
{
	// Level 이 존재하지 않으면 Rendering X
	if (!CLevelManager::GetInst()->GetCurrentLevel())
		return;

	// Swap Chain 의 Back Buffer 를 Render Target 으로 재지정
	Ptr<CTexture> pRTTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	Ptr<CTexture> pDSTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
	CONTEXT->OMSetRenderTargets(1, pRTTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());

	// 렌더링에 필요한 데이터 바인딩
	DataBinding();

	// Target Clear
	float clearColor[4] = { 0.3f, 0.3f, 0.3f, 1.f };
	CDevice::GetInst()->ClearTarget(clearColor);

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
	if(m_EditorCam != nullptr)
		m_EditorCam->Render();
}

void CRenderManager::DataBinding()
{
	// Global Data Binding
	g_GlobalData.Light2DCount = (int)m_vecLight2D.size();
	//g_GlobalData.Light3DCount = (int)m_vecLight3D.size();

	static CConstBuffer* GlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	GlobalBuffer->SetData(&g_GlobalData);
	GlobalBuffer->Binding();
	GlobalBuffer->Binding_CS();


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
}

void CRenderManager::DataClear()
{
	m_vecLight2D.clear();
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
