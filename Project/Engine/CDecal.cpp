#include "pch.h"
#include "CDecal.h"

#include "CTransform.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
	, m_Alpha(1.f)
{
	SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"CubeMesh"));
	SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DecalMaterial"));

	GetMaterial()->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
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
		GetMaterial()->SetTexParam(TEX_1, m_OutputTex);

	GetMaterial()->SetScalarParam(FLOAT_0, m_Alpha);

	GetMaterial()->Binding();

	GetMesh()->Binding();
}