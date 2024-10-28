#pragma once
#include "CComponent.h"

#include "CFrustum.h"

class CCamera : public CComponent
{
private:
	CFrustum	m_Frustum;

	PROJ_TYPE	m_ProjType;

	int			m_CamPriority;	// Render Manager 에 등록될때 카메라의 우선순위 ( 0 : Main Camera , -1 : 미등록 카메라, 0 보다 크다 : Sub Camera )

	float		m_FOV;			// Field of View (카메라 시야각)
	float		m_Far;			// 카메라가 볼 수 있는 최대 거리

	float		m_Width;		// 시야 가로 길이
	float		m_AspectRatio;	// 종횡비

	float		m_Scale;		// 직교투영 배율

	// View Space 의 특징
	// 1. Camera 가 원점이다
	// 2. Camera 는 Z 축을 바라보고 있다.
	Matrix		m_matView;
	Matrix		m_matViewInv;
	Matrix		m_matProj;		// 투영
	Matrix		m_matProjInv;

	// Check 한 Layer 만 볼 수 있게 (필터 처리)
	// 오브젝츠 계층 관계에서 부모 Layer 를 본다고 해도
	// 자식 오브젝트는 Layer 가 다를 수 있다.
	UINT		m_LayerCheck;
	tRay		m_Ray;

	//vector<CGameObject*>	m_vecDeferred; -> Instancing 추가로 세분화
	// 물체 분류 세분화
	map<ULONG64, vector<tInstObject>> m_mapInstGroup_D;		// Deferred Object
	map<ULONG64, vector<tInstObject>> m_mapInstGroup_F;		// Foward Object (Opaque, Mask)
	map<INT_PTR, vector<tInstObject>> m_mapSingleObject;	// Single Object

	vector<CGameObject*>	m_vecDecal;

	vector<CGameObject*>	m_vecOpaque;
	vector<CGameObject*>	m_vecMasked;
	vector<CGameObject*>	m_vecTransParent;
	vector<CGameObject*>	m_vecParticle;
	vector<CGameObject*>	m_vecPostProcess;

	// Shadow Map
	vector<CGameObject*>	m_vecShadowMap;

public:
	void SetCameraPriority(int priority);

	void SetProjType(PROJ_TYPE type) { m_ProjType = type; }
	void SetFar(float _far) { m_Far = _far; }
	void SetFOV(float _fov) { m_FOV = _fov; }
	void SetWidth(float width) { m_Width = width; }
	void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; }
	void SetScale(float scale) { m_Scale = scale; }

	int GetCameraPriority() { return m_CamPriority; }
	PROJ_TYPE GetProjType() { return m_ProjType; }
	float GetFar() { return m_Far; }
	float GetFOV() { return m_FOV; }
	float GetWidth() { return m_Width; }
	float GetAspectRatio() { return m_AspectRatio; }
	float GetScale() { return m_Scale; }
	const tRay& GetRay() { return m_Ray; }

	const Matrix& GetViewMat() { return m_matView; }
	const Matrix& GetViewInvMat() { return m_matViewInv; }
	const Matrix& GetProjMat() { return m_matProj; }
	const Matrix& GetProjInvMat() { return m_matProjInv; }

	void LayerCheck(int layerIdx);
	void LayerCheckAll(UINT value) { m_LayerCheck = value; } // 모든 숫자에 1 을 넣는
	const UINT GetLayerCheck() { return m_LayerCheck; }

	void SortObject();
	void SortObject_ShadowMap();

public:
	virtual void Begin() override;
	virtual	void FinalTick() override; // 반드시 구현
	void Render();

private:
	void CalcRay();

public:
	void SortClear();

	void Render_deferred();
	void Render_decal();
	void Render_opaque();
	void Render_masked();
	void Render_transparent();
	void Render_particle();
	void Render_postprocess();

	void Render_shadowmap();

	void UpdateReflectionTexture(Ptr<CTexture> reflectionTexture, float waterHeight);

public:
	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CCamera);

public:
	CCamera();
	CCamera(const CCamera& other);
	~CCamera();
};

