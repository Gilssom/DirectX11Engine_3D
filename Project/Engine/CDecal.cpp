#include "pch.h"
#include "CDecal.h"

#include "CDevice.h"
#include "CTransform.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
	, m_AsLight(false)
	, m_LightPower(0.4f)
{
	SetDynamicShadow(false);

	SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"CubeMesh"));
	SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DecalMaterial"), 0);

	GetMaterial(0)->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
}

CDecal::~CDecal()
{

}

void CDecal::FinalTick()
{
	DrawDebugCube(Transform()->GetWorldMat(), Vec4(0.f, 1.f, 0.f, 1.f), true, 0.f);
}

void CDecal::Render()
{
	Transform()->Binding();

	if (m_OutputTex != nullptr)
		GetMaterial(0)->SetTexParam(TEX_1, m_OutputTex);

	GetMaterial(0)->SetScalarParam(INT_0, (int)m_AsLight);
	GetMaterial(0)->SetScalarParam(FLOAT_0, m_LightPower);
	GetMaterial(0)->Binding();

	GetMesh()->Render(0);
}