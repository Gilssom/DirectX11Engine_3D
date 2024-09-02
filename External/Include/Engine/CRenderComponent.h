#pragma once
#include "CComponent.h"

#include "assets.h"

struct tMtrlSet
{
	Ptr<CMaterial>  pSharedMtrl;    // 공유 메테리얼
	Ptr<CMaterial>  pDynamicMtrl;   // 공유 메테리얼의 복사본    
	Ptr<CMaterial>  pCurMtrl;       // 현재 사용 할 메테리얼
};

class CRenderComponent : public CComponent
{
private:
	Ptr<CMesh>			m_Mesh;		// 모양

	// 앞으로 Shader, Texture 는 Material 이 관리함 ( Unity Material 과 비슷한 개념 )
	// 사용하고자 하는 Shader 의 종류에 따라 요구하는 Texture 의 갯수가 달라질 수 있음

	//Ptr<CMaterial>		m_CurMaterial;		
	//Ptr<CMaterial>		m_SharedMaterial;
	//Ptr<CMaterial>		m_DynamicMaterial;	

	vector<tMtrlSet>    m_vecMtrls; // 재질

	bool				m_FrustumCheck;		// Frustum Culling 적용 여부

	bool				m_DynamicShadow;	// 동적그림자 생성 여부

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

	// 동적 재질을 생성할 수 있는 조건은 해당 Level 이 Play 상태이여야 한다.
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

