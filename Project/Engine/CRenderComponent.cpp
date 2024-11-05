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

		// 원본 오브젝트가 공유재질을 참조하고 있고, 현재 사용재질은 공유재질이 아닌경우
		if (other.m_vecMtrls[i].pSharedMtrl != other.m_vecMtrls[i].pCurMtrl)
		{
			assert(other.m_vecMtrls[i].pDynamicMtrl.Get());

			// 복사 렌더 컴포넌트도 별도의 동적재질을 생성한다.
			GetDynamicMaterial(i);

			// 원본 렌더컴포넌트의 동적재질 값을 현재 생성한 동적재질로 복사한다.
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
	// 동적 재질을 세팅받을 수 없다.
	assert(!material->IsDynamic());

	// 재질이 변경되면 기존에 복사본을 받아둔 DynamicMaterial 을 삭제한다.
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
	// 공유재질을 가져오는 것으로, 현재 사용재질을 동적재질에서 회복하도록 한다.
	m_vecMtrls[idx].pCurMtrl = m_vecMtrls[idx].pSharedMtrl;

	if (m_vecMtrls[idx].pDynamicMtrl.Get())
	{
		m_vecMtrls[idx].pDynamicMtrl = nullptr;
	}

	return m_vecMtrls[idx].pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(UINT idx)
{
	// 동적 재질을 생성할 수 있는 조건은 해당 Level 이 Play 상태이여야 한다.
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
	if (pCurLevel->GetState() != LEVEL_STATE::PLAY)
		return nullptr;

	// 원본 재질이 없다 -> nullptr 반환
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
		Animator3D()->Binding(true); // 애니메이션 활성화

		for (UINT i = 0; i < GetMaterialCount(); ++i)
		{
			if (nullptr == GetMaterial(i))
				continue;

			GetMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
			GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	Ptr<CMaterial> pShadowMapMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"ShadowMapMaterial");

	if (Animator3D())
	{
		pShadowMapMaterial->SetScalarParam(INT_0, 1); // 애니메이션 활성화
	}
	else
	{
		pShadowMapMaterial->SetScalarParam(INT_0, 0); // 애니메이션 비활성화
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
	// 메쉬 참조정보 저장
	SaveAssetRef(m_Mesh, file);

	// 재질 참조정보 저장
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
	// 메쉬 참조정보 불러오기
	LoadAssetRef(m_Mesh, file);

	// 재질 참조정보 불러오기
	UINT iMtrlCount = GetMaterialCount();
	fread(&iMtrlCount, sizeof(UINT), 1, file);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		LoadAssetRef(m_vecMtrls[i].pSharedMtrl, file);
	}

	fread(&m_DynamicShadow, 1, 1, file);
}