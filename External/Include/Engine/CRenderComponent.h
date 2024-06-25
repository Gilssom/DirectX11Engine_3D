#pragma once
#include "CComponent.h"

#include "assets.h"

class CRenderComponent : public CComponent
{
private:
	Ptr<CMesh>			m_Mesh;		// 모양

	// 앞으로 Shader, Texture 는 Material 이 관리함 ( Unity Material 과 비슷한 개념 )
	// 사용하고자 하는 Shader 의 종류에 따라 요구하는 Texture 의 갯수가 달라질 수 있음

	Ptr<CMaterial>		m_CurMaterial;		// 현재 사용중인 재질을 참조
	Ptr<CMaterial>		m_SharedMaterial;	// Asset Manager 에 등록된 재질을 참조
	Ptr<CMaterial>		m_DynamicMaterial;	// 원본 재질의 사본 (복사본)

public:
	virtual void Render() = 0;

public:
	void SetMesh(Ptr<CMesh> mesh) { m_Mesh = mesh; }
	Ptr<CMesh> GetMesh() { return m_Mesh; }

	void SetMaterial(Ptr<CMaterial> material);
	Ptr<CMaterial> GetMaterial() { return m_CurMaterial; }

	// 동적 재질을 생성할 수 있는 조건은 해당 Level 이 Play 상태이여야 한다.
	Ptr<CMaterial> GetDynamicMaterial();
	void RestoreMaterial();

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

public:
	CRenderComponent(COMPONENT_TYPE type);
	CRenderComponent(const CRenderComponent& other);
	~CRenderComponent();
};

