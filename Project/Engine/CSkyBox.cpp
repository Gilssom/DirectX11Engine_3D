#include "pch.h"
#include "CSkyBox.h"

#include "CTransform.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_Type(SKYBOX_TYPE::SPHERE)
{
	SetSkyBoxType(m_Type);

	SetFrustumCheck(false);
	SetDynamicShadow(false);
}

CSkyBox::~CSkyBox()
{

}

void CSkyBox::FinalTick()
{

}

void CSkyBox::Render()
{
	// ���� Mesh �� Material �� ���ٸ� (���� ������ ������ ���� ������ ��ȯ��)
	if (GetMesh() == nullptr || GetMaterial(0) == nullptr)
	{
		return;
	}

	// ������Ʈ�� ��ġ���� ������۸� ���ؼ� ���ε�
	Transform()->Binding();

	// ����� ���׸���(���̴�, �ؽ�ó) ���ε�
	GetMaterial(0)->SetScalarParam(INT_0, m_Type);

	if (m_Type == SKYBOX_TYPE::SPHERE)
		GetMaterial(0)->SetTexParam(TEX_0, m_SkyBoxTexture);
	else if(m_Type == SKYBOX_TYPE::CUBE)
		GetMaterial(0)->SetTexParam(TEX_CUBE_0, m_SkyBoxTexture);

	GetMaterial(0)->Binding();

	// Mesh , Shader Render
	GetMesh()->Render(0);
}

void CSkyBox::SetSkyBoxType(SKYBOX_TYPE type)
{
	// Type ���� Texture Type �� �ٸ��� ������
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

	SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"SkyBoxMaterial"), 0);
}