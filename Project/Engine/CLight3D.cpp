#include "pch.h"
#include "CLight3D.h"

#include "CRenderManager.h"
#include "CTransform.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_Info{}
	, m_LightIdx(-1)
{

}

CLight3D::~CLight3D()
{

}

void CLight3D::FinalTick()
{
	m_Info.WorldPos = Transform()->GetWorldPos();
	m_Info.WorldDir = Transform()->GetWorldDir(DIR_TYPE::FRONT); // 3D 공간의 정면 방향

	m_LightIdx = CRenderManager::GetInst()->RegisterLight3D(this);

	// Debug Render 요청
	if (m_Info.LightType == (UINT)LIGHT_TYPE::POINT)
		DrawDebugSphere(m_Info.WorldPos, m_Info.Range, Vec4(1.f, 1.f, 0.f, 1.f), false, 0.f);
	else
		DrawDebugCube(m_Info.WorldPos, Vec3(50.f, 50.f, 200.f), Transform()->GetRelativeRotation(), Vec4(1.f, 1.f, 0.f, 1.f), true, 0.f);
}

void CLight3D::Lighting()
{
	m_LightMaterial->SetScalarParam(INT_0, m_LightIdx);
	m_LightMaterial->Binding();

	if ((LIGHT_TYPE)m_Info.LightType == LIGHT_TYPE::POINT)
	{
		Transform()->Binding();
	}

	m_VolumeMesh->Render();
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
		// Mesh 및 Material 내부 설정
		m_VolumeMesh = CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh");
		m_LightMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"DirLightMaterial");

		// Parameter 내부 설정
		m_LightMaterial->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
		m_LightMaterial->SetTexParam(TEX_1, CAssetManager::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));
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