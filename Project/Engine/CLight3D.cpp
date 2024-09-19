#include "pch.h"
#include "CLight3D.h"

#include "CRenderManager.h"
#include "CTransform.h"
#include "CCamera.h"
#include "CMRT.h"

#include "CAssetManager.h"
#include "CTexture.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_Info{}
	, m_LightIdx(-1)
	, m_LightCamObject(nullptr)
	, m_ShadowMapMRT(nullptr)
{
	// �������� Shadow Map �� ����� ���� ����� ī�޶�
	m_LightCamObject = new CGameObject;
	m_LightCamObject->AddComponent(new CTransform);
	m_LightCamObject->AddComponent(new CCamera);
}

CLight3D::CLight3D(const CLight3D& origin)
	: CComponent(origin)
	, m_Info(origin.m_Info)
	, m_LightIdx(-1)
	, m_LightCamObject(nullptr)
	, m_ShadowMapMRT(nullptr)
{

}

CLight3D::~CLight3D()
{
	if (m_ShadowMapMRT != nullptr)
		delete m_ShadowMapMRT;

	if (m_LightCamObject != nullptr)
		delete m_LightCamObject;
}

void CLight3D::FinalTick()
{
	m_Info.WorldPos = Transform()->GetWorldPos();
	m_Info.WorldDir = Transform()->GetWorldDir(DIR_TYPE::FRONT); // 3D ������ ���� ����

	m_LightIdx = CRenderManager::GetInst()->RegisterLight3D(this);

	// Debug Render ��û
	if (m_Info.LightType == (UINT)LIGHT_TYPE::POINT)
		DrawDebugSphere(m_Info.WorldPos, m_Info.Range, Vec4(1.f, 1.f, 0.f, 1.f), false, 0.f);
	else if(m_Info.LightType == (UINT)LIGHT_TYPE::SPOT)
		DrawDebugCone(m_Info.WorldPos, Transform()->GetRelativeScale(), Transform()->GetRelativeRotation(), Vec4(1.f, 1.f, 0.f, 1.f), false, 0.f);
	else
		DrawDebugCube(m_Info.WorldPos, Vec3(50.f, 50.f, 200.f), Transform()->GetRelativeRotation(), Vec4(1.f, 1.f, 0.f, 1.f), true, 0.f);


	// ���� ī�޶� ���� ����
	*m_LightCamObject->Transform() = *Transform();
	m_LightCamObject->Camera()->FinalTick();
}

void CLight3D::Render_ShadowMap()
{
	if ((LIGHT_TYPE)m_Info.LightType != LIGHT_TYPE::DIRECTIONAL)
		return;

	// Shadow Map ���� ��ü
	m_ShadowMapMRT->ClearTarget();
	m_ShadowMapMRT->ClearDepthStencil();
	m_ShadowMapMRT->OMSet();

	// ���� ������ Camera �� ����
	CCamera* pLightCam = m_LightCamObject->Camera();

	g_Trans.matView		= pLightCam->GetViewMat();
	g_Trans.matViewInv	= pLightCam->GetViewInvMat();
	g_Trans.matProj		= pLightCam->GetProjMat();
	g_Trans.matProjInv	= pLightCam->GetProjInvMat();

	// ���� Camera �� ���� �׸��ڰ� ���ܾ� �ϴ� ��ü�� �з� �۾�
	pLightCam->SortObject_ShadowMap();
	pLightCam->Render_shadowmap();
}

void CLight3D::Lighting()
{
	Binding();
	m_VolumeMesh->Render(0);
}

void CLight3D::Binding()
{
	m_LightMaterial->SetScalarParam(INT_0, m_LightIdx);

	if ((LIGHT_TYPE)m_Info.LightType == LIGHT_TYPE::DIRECTIONAL)
	{
		Ptr<CTexture> pRTTex = m_ShadowMapMRT->GetRenderTarget(0);
		m_LightMaterial->SetTexParam(TEX_2, pRTTex);

		Matrix matVP = m_LightCamObject->Camera()->GetViewMat() * m_LightCamObject->Camera()->GetProjMat();
		m_LightMaterial->SetScalarParam(MAT_0, matVP);
	}

	if ((LIGHT_TYPE)m_Info.LightType == LIGHT_TYPE::POINT)
	{
		Transform()->Binding();
	}
	else if ((LIGHT_TYPE)m_Info.LightType == LIGHT_TYPE::SPOT)
	{
		Transform()->Binding();
	}

	m_LightMaterial->Binding();
}

void CLight3D::SetRange(float range)
{
	m_Info.Range = range;
	Transform()->SetRelativeScale(m_Info.Range * 2.f, m_Info.Range * 2.f, m_Info.Range * 2.f);
}

void CLight3D::SetLightType(LIGHT_TYPE type)
{
	m_Info.LightType = (UINT)type;

	if ((LIGHT_TYPE)m_Info.LightType == LIGHT_TYPE::DIRECTIONAL)
	{
		// Mesh �� Material ���� ����
		m_VolumeMesh = CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh");
		m_LightMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"DirLightMaterial");

		// Parameter ���� ����
		m_LightMaterial->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
		m_LightMaterial->SetTexParam(TEX_1, CAssetManager::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));

		if (m_ShadowMapMRT != nullptr)
		{
			delete m_ShadowMapMRT;
			m_ShadowMapMRT = nullptr;
		}

		m_ShadowMapMRT = new CMRT;

		// 1. ShadowMap Target
		Ptr<CTexture> pShadowMap = CAssetManager::GetInst()->CreateTexture(L"ShadowMapTargetTex"
									, 8192, 8192
									, DXGI_FORMAT_R32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		// Depth Stencil Texture ����
		Ptr<CTexture> DSTex = CAssetManager::GetInst()->CreateTexture(L"ShadowMapDSTex"
									, 8192, 8192
									, DXGI_FORMAT_D24_UNORM_S8_UINT
									, D3D11_BIND_DEPTH_STENCIL);

		m_ShadowMapMRT->Create(&pShadowMap, 1, DSTex);

		// ������ ��ġ�� ������� ������ �������� �׸��ڰ� ���;� �Ѵ�. (��������)
		m_LightCamObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		m_LightCamObject->Camera()->SetWidth(8192);
		m_LightCamObject->Camera()->SetAspectRatio(1.f);
		m_LightCamObject->Camera()->SetFar(100000.f);
		m_LightCamObject->Camera()->LayerCheckAll(0xffffffff);
	}
	else if ((LIGHT_TYPE)m_Info.LightType == LIGHT_TYPE::POINT)
	{
		m_VolumeMesh = CAssetManager::GetInst()->FindAsset<CMesh>(L"SphereMesh");
		m_LightMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"PointLightMaterial");

		m_LightMaterial->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
		m_LightMaterial->SetTexParam(TEX_1, CAssetManager::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));
	}
	else if ((LIGHT_TYPE)m_Info.LightType == LIGHT_TYPE::SPOT)
	{
		m_VolumeMesh = CAssetManager::GetInst()->FindAsset<CMesh>(L"ConeMesh");
		m_LightMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"SpotLightMaterial");

		m_LightMaterial->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
		m_LightMaterial->SetTexParam(TEX_1, CAssetManager::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));
	}
}

void CLight3D::SaveToLevelFile(FILE* file)
{
	fwrite(&m_Info, sizeof(tLightInfo), 1, file);
}

void CLight3D::LoadFromLevelFile(FILE* file)
{
	fread(&m_Info, sizeof(tLightInfo), 1, file);
}