#include "pch.h"
#include "CRenderComponent.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE type)
	: CComponent(type)
{

}

CRenderComponent::CRenderComponent(const CRenderComponent& other)
	: CComponent(other)
	, m_Mesh(other.m_Mesh)
	, m_CurMaterial(nullptr)
	, m_SharedMaterial(other.m_SharedMaterial)
	, m_DynamicMaterial(nullptr)
{
	// 원본 Object 가 동적 재질을 가지고 있었고, 그걸 지금 사용 중이라면 복제
	if (other.m_DynamicMaterial != nullptr && other.m_DynamicMaterial == other.m_CurMaterial)
	{
		// 복사되는 Object 도 자신만의 동적 재질을 가져야 하고,
		GetDynamicMaterial();

		// 원본 Object 의 동적 재질 Setting 값을 똑같이 받아와야 한다.
		m_DynamicMaterial = other.m_DynamicMaterial;
	}
	
	// 원본 Object 의 현재 사용 재질이 Shared Material 이면, 복사본도 똑같이 지정해준다.
	else if (other.m_SharedMaterial != nullptr && other.m_SharedMaterial == other.m_CurMaterial)
	{
		m_CurMaterial = other.m_SharedMaterial;
	}
}

CRenderComponent::~CRenderComponent()
{

}

void CRenderComponent::SetMaterial(Ptr<CMaterial> material)
{
	if (material == nullptr)
	{
		m_CurMaterial = m_SharedMaterial = material;
		return;
	}
	// 동적 재질을 세팅받을 수 없다.
	assert(!material->IsDynamic());

	m_CurMaterial = m_SharedMaterial = material;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	// 동적 재질을 생성할 수 있는 조건은 해당 Level 이 Play 상태이여야 한다.


	// 재질 자체를 참조한 적이 없다면 nullptr 반환
	if (m_SharedMaterial == nullptr)
		return nullptr;

	// 동적 재질을 이전에 생성을 했었다면 반환
	if (m_DynamicMaterial != nullptr)
	{
		m_CurMaterial = m_DynamicMaterial;
		return m_DynamicMaterial;
	}
	
	// 현재 사용되고 있는 재질도 동적 재질로 변경
	m_CurMaterial = m_DynamicMaterial = m_SharedMaterial->GetDynamicMaterial();

	return m_DynamicMaterial;
}

void CRenderComponent::RestoreMaterial()
{
	// Shared 재질로 다시 복구
	m_CurMaterial = m_SharedMaterial;
	m_DynamicMaterial = nullptr;
}

void CRenderComponent::SaveToLevelFile(FILE* file)
{
	SaveAssetRef(m_Mesh, file);
	SaveAssetRef(m_SharedMaterial, file);
}

void CRenderComponent::LoadFromLevelFile(FILE* file)
{
	LoadAssetRef(m_Mesh, file);
	LoadAssetRef(m_SharedMaterial, file);

	SetMaterial(m_SharedMaterial);
}