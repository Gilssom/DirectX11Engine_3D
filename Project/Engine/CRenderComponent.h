#pragma once
#include "CComponent.h"

#include "assets.h"

struct tMtrlSet
{
	Ptr<CMaterial>  pSharedMtrl;    // ���� ���׸���
	Ptr<CMaterial>  pDynamicMtrl;   // ���� ���׸����� ���纻    
	Ptr<CMaterial>  pCurMtrl;       // ���� ��� �� ���׸���
};

class CRenderComponent : public CComponent
{
private:
	Ptr<CMesh>			m_Mesh;		// ���

	// ������ Shader, Texture �� Material �� ������ ( Unity Material �� ����� ���� )
	// ����ϰ��� �ϴ� Shader �� ������ ���� �䱸�ϴ� Texture �� ������ �޶��� �� ����

	//Ptr<CMaterial>		m_CurMaterial;		
	//Ptr<CMaterial>		m_SharedMaterial;
	//Ptr<CMaterial>		m_DynamicMaterial;	

	vector<tMtrlSet>    m_vecMtrls; // ����

	bool				m_FrustumCheck;		// Frustum Culling ���� ����

	bool				m_DynamicShadow;	// �����׸��� ���� ����

public:
	virtual void Render() = 0;
	virtual void Render(UINT iSubset);
	virtual void Render_shadowmap();

public:
	void SetMesh(Ptr<CMesh> mesh);
	Ptr<CMesh> GetMesh() { return m_Mesh; }

	void SetMaterial(Ptr<CMaterial> material, UINT idx);
	Ptr<CMaterial> GetMaterial(UINT idx);
	Ptr<CMaterial> GetSharedMaterial(UINT idx);

	UINT GetMaterialCount() { return (UINT)m_vecMtrls.size(); }

	// ���� ������ ������ �� �ִ� ������ �ش� Level �� Play �����̿��� �Ѵ�.
	Ptr<CMaterial> GetDynamicMaterial(UINT idx);

	void SetFrustumCheck(bool check) { m_FrustumCheck = check; }
	bool IsFrustumCheck() { return m_FrustumCheck; }

	void SetDynamicShadow(bool use) { m_DynamicShadow = use; }
	bool IsDynamicShadow() { return m_DynamicShadow; }

	ULONG64 GetInstID(UINT iMtrlIdx);

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

public:
	CRenderComponent(COMPONENT_TYPE type);
	CRenderComponent(const CRenderComponent& other);
	~CRenderComponent();
};

