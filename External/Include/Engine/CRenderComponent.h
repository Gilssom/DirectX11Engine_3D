#pragma once
#include "CComponent.h"

#include "assets.h"

class CRenderComponent : public CComponent
{
private:
	Ptr<CMesh>			m_Mesh;		// ���

	// ������ Shader, Texture �� Material �� ������ ( Unity Material �� ����� ���� )
	// ����ϰ��� �ϴ� Shader �� ������ ���� �䱸�ϴ� Texture �� ������ �޶��� �� ����

	Ptr<CMaterial>		m_CurMaterial;		// ���� ������� ������ ����
	Ptr<CMaterial>		m_SharedMaterial;	// Asset Manager �� ��ϵ� ������ ����
	Ptr<CMaterial>		m_DynamicMaterial;	// ���� ������ �纻 (���纻)

public:
	virtual void Render() = 0;

public:
	void SetMesh(Ptr<CMesh> mesh) { m_Mesh = mesh; }
	Ptr<CMesh> GetMesh() { return m_Mesh; }

	void SetMaterial(Ptr<CMaterial> material);
	Ptr<CMaterial> GetMaterial() { return m_CurMaterial; }

	// ���� ������ ������ �� �ִ� ������ �ش� Level �� Play �����̿��� �Ѵ�.
	Ptr<CMaterial> GetDynamicMaterial();
	void RestoreMaterial();

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

public:
	CRenderComponent(COMPONENT_TYPE type);
	CRenderComponent(const CRenderComponent& other);
	~CRenderComponent();
};

