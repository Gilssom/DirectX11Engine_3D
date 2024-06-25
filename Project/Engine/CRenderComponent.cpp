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
	// ���� Object �� ���� ������ ������ �־���, �װ� ���� ��� ���̶�� ����
	if (other.m_DynamicMaterial != nullptr && other.m_DynamicMaterial == other.m_CurMaterial)
	{
		// ����Ǵ� Object �� �ڽŸ��� ���� ������ ������ �ϰ�,
		GetDynamicMaterial();

		// ���� Object �� ���� ���� Setting ���� �Ȱ��� �޾ƿ;� �Ѵ�.
		m_DynamicMaterial = other.m_DynamicMaterial;
	}
	
	// ���� Object �� ���� ��� ������ Shared Material �̸�, ���纻�� �Ȱ��� �������ش�.
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
	// ���� ������ ���ù��� �� ����.
	assert(!material->IsDynamic());

	m_CurMaterial = m_SharedMaterial = material;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	// ���� ������ ������ �� �ִ� ������ �ش� Level �� Play �����̿��� �Ѵ�.


	// ���� ��ü�� ������ ���� ���ٸ� nullptr ��ȯ
	if (m_SharedMaterial == nullptr)
		return nullptr;

	// ���� ������ ������ ������ �߾��ٸ� ��ȯ
	if (m_DynamicMaterial != nullptr)
	{
		m_CurMaterial = m_DynamicMaterial;
		return m_DynamicMaterial;
	}
	
	// ���� ���ǰ� �ִ� ������ ���� ������ ����
	m_CurMaterial = m_DynamicMaterial = m_SharedMaterial->GetDynamicMaterial();

	return m_DynamicMaterial;
}

void CRenderComponent::RestoreMaterial()
{
	// Shared ������ �ٽ� ����
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