#include "pch.h"
#include "CSkyBox.h"

#include "CTransform.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_Type(SKYBOX_TYPE::SPHERE)
{
	SetSkyBoxType(m_Type);

	SetFrustumCheck(false);
}

CSkyBox::~CSkyBox()
{

}

void CSkyBox::FinalTick()
{

}

void CSkyBox::Render()
{
	// 만약 Mesh 나 Material 이 없다면 (동적 재질이 있으면 동적 재질을 반환함)
	if (GetMesh() == nullptr || GetMaterial() == nullptr)
	{
		return;
	}

	// 오브젝트의 위치값을 상수버퍼를 통해서 바인딩
	Transform()->Binding();

	// 사용할 머테리얼(쉐이더, 텍스처) 바인딩
	GetMaterial()->SetScalarParam(INT_0, m_Type);

	if (m_Type == SKYBOX_TYPE::SPHERE)
		GetMaterial()->SetTexParam(TEX_0, m_SkyBoxTexture);
	else if(m_Type == SKYBOX_TYPE::CUBE)
		GetMaterial()->SetTexParam(TEX_CUBE_0, m_SkyBoxTexture);

	GetMaterial()->Binding();

	// Mesh , Shader Render
	GetMesh()->Render();
}

void CSkyBox::SetSkyBoxType(SKYBOX_TYPE type)
{
	// Type 간의 Texture Type 이 다르기 때문에
	if (m_Type != type)
		m_SkyBoxTexture = nullptr;

	m_Type = type;

	if (m_Type == SKYBOX_TYPE::SPHERE)
	{
		SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	}
	else if (m_Type == SKYBOX_TYPE::CUBE)
	{
		SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"CubeMesh"));
	}

	SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"SkyBoxMaterial"));
}