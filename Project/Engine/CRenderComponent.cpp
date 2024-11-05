#include "pch.h"
#include "CRenderComponent.h"

#include "CLevelManager.h"
#include "CLevel.h"
#include "CTransform.h"
#include "CAnimator3D.h"
#include "CMeshRender.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE type)
	: CComponent(type)
	, m_FrustumCheck(true)
	, m_DynamicShadow(true)
{

}

CRenderComponent::CRenderComponent(const CRenderComponent& other)
	: CComponent(other)
	, m_Mesh(other.m_Mesh)
	, m_FrustumCheck(other.m_FrustumCheck)
	, m_DynamicShadow(other.m_DynamicShadow)
{
	m_vecMtrls.resize(other.m_vecMtrls.size());

	for (size_t i = 0; i < other.m_vecMtrls.size(); ++i)
	{
		m_vecMtrls[i].pCurMtrl = other.m_vecMtrls[i].pCurMtrl;
		m_vecMtrls[i].pSharedMtrl = other.m_vecMtrls[i].pSharedMtrl;

		// ���� ������Ʈ�� ���������� �����ϰ� �ְ�, ���� ��������� ���������� �ƴѰ��
		if (other.m_vecMtrls[i].pSharedMtrl != other.m_vecMtrls[i].pCurMtrl)
		{
			assert(other.m_vecMtrls[i].pDynamicMtrl.Get());

			// ���� ���� ������Ʈ�� ������ ���������� �����Ѵ�.
			GetDynamicMaterial(i);

			// ���� ����������Ʈ�� �������� ���� ���� ������ ���������� �����Ѵ�.
			*m_vecMtrls[i].pDynamicMtrl.Get() = *other.m_vecMtrls[i].pDynamicMtrl.Get();
		}
		else
		{
			m_vecMtrls[i].pCurMtrl = m_vecMtrls[i].pSharedMtrl;
		}
	}
}

CRenderComponent::~CRenderComponent()
{

}

void CRenderComponent::SetMesh(Ptr<CMesh> mesh)
{
	m_Mesh = mesh;

	if (!m_vecMtrls.empty())
	{
		m_vecMtrls.clear();
		vector<tMtrlSet> vecMtrls;
		m_vecMtrls.swap(vecMtrls);
	}

	if (m_Mesh != nullptr)
		m_vecMtrls.resize(m_Mesh->GetSubsetCount());
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> material, UINT idx)
{
	// ���� ������ ���ù��� �� ����.
	assert(!material->IsDynamic());

	// ������ ����Ǹ� ������ ���纻�� �޾Ƶ� DynamicMaterial �� �����Ѵ�.
	m_vecMtrls[idx].pSharedMtrl = material;
	m_vecMtrls[idx].pCurMtrl = material;
	m_vecMtrls[idx].pDynamicMtrl = nullptr;
}

Ptr<CMaterial> CRenderComponent::GetMaterial(UINT idx)
{
	return m_vecMtrls[idx].pCurMtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(UINT idx)
{
	// ���������� �������� ������, ���� ��������� ������������ ȸ���ϵ��� �Ѵ�.
	m_vecMtrls[idx].pCurMtrl = m_vecMtrls[idx].pSharedMtrl;

	if (m_vecMtrls[idx].pDynamicMtrl.Get())
	{
		m_vecMtrls[idx].pDynamicMtrl = nullptr;
	}

	return m_vecMtrls[idx].pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(UINT idx)
{
	// ���� ������ ������ �� �ִ� ������ �ش� Level �� Play �����̿��� �Ѵ�.
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
	if (pCurLevel->GetState() != LEVEL_STATE::PLAY)
		return nullptr;

	// ���� ������ ���� -> nullptr ��ȯ
	if (nullptr == m_vecMtrls[idx].pSharedMtrl)
	{
		m_vecMtrls[idx].pCurMtrl = nullptr;
		m_vecMtrls[idx].pDynamicMtrl = nullptr;
		return m_vecMtrls[idx].pCurMtrl;
	}

	if (nullptr == m_vecMtrls[idx].pDynamicMtrl)
	{
		m_vecMtrls[idx].pDynamicMtrl = m_vecMtrls[idx].pSharedMtrl->Clone();
		m_vecMtrls[idx].pDynamicMtrl->SetName(m_vecMtrls[idx].pSharedMtrl->GetName() + L"_Clone");
		m_vecMtrls[idx].pCurMtrl = m_vecMtrls[idx].pDynamicMtrl;
	}

	return m_vecMtrls[idx].pCurMtrl;
}

void CRenderComponent::Render(UINT iSubset)
{
	Render();
}

void CRenderComponent::Render_shadowmap()
{
	Transform()->Binding();

	if (Animator3D())
	{
		Animator3D()->Binding(true); // �ִϸ��̼� Ȱ��ȭ

		for (UINT i = 0; i < GetMaterialCount(); ++i)
		{
			if (nullptr == GetMaterial(i))
				continue;

			GetMaterial(i)->SetAnim3D(true); // Animation Mesh �˸���
			GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	Ptr<CMaterial> pShadowMapMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"ShadowMapMaterial");

	if (Animator3D())
	{
		pShadowMapMaterial->SetScalarParam(INT_0, 1); // �ִϸ��̼� Ȱ��ȭ
	}
	else
	{
		pShadowMapMaterial->SetScalarParam(INT_0, 0); // �ִϸ��̼� ��Ȱ��ȭ
	}

	pShadowMapMaterial->Binding();

	for (int i = 0; i < GetMaterialCount(); i++)
	{
		GetMesh()->Render(i);
	}
}

ULONG64 CRenderComponent::GetInstID(UINT iMtrlIdx)
{
	if (m_Mesh == nullptr || m_vecMtrls[iMtrlIdx].pCurMtrl == nullptr)
		return 0;

	uInstID id{ (UINT)m_Mesh->GetID(), (WORD)m_vecMtrls[iMtrlIdx].pCurMtrl->GetID(), (WORD)iMtrlIdx };
	return id.LLID;
}

void CRenderComponent::SaveToLevelFile(FILE* file)
{
	// �޽� �������� ����
	SaveAssetRef(m_Mesh, file);

	// ���� �������� ����
	UINT iMtrlCount = GetMaterialCount();
	fwrite(&iMtrlCount, sizeof(UINT), 1, file);

	for (UINT i = 0; i < iMtrlCount; i++)
	{
		SaveAssetRef(m_vecMtrls[i].pSharedMtrl, file);
	}

	fwrite(&m_DynamicShadow, 1, 1, file);
}

void CRenderComponent::LoadFromLevelFile(FILE* file)
{
	// �޽� �������� �ҷ�����
	LoadAssetRef(m_Mesh, file);

	// ���� �������� �ҷ�����
	UINT iMtrlCount = GetMaterialCount();
	fread(&iMtrlCount, sizeof(UINT), 1, file);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		LoadAssetRef(m_vecMtrls[i].pSharedMtrl, file);
	}

	fread(&m_DynamicShadow, 1, 1, file);
}