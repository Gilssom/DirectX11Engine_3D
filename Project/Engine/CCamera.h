#pragma once
#include "CComponent.h"

#include "CFrustum.h"

class CCamera : public CComponent
{
private:
	CFrustum	m_Frustum;

	PROJ_TYPE	m_ProjType;

	int			m_CamPriority;	// Render Manager �� ��ϵɶ� ī�޶��� �켱���� ( 0 : Main Camera , -1 : �̵�� ī�޶�, 0 ���� ũ�� : Sub Camera )

	float		m_FOV;			// Field of View (ī�޶� �þ߰�)
	float		m_Far;			// ī�޶� �� �� �ִ� �ִ� �Ÿ�

	float		m_Width;		// �þ� ���� ����
	float		m_AspectRatio;	// ��Ⱦ��

	float		m_Scale;		// �������� ����

	// View Space �� Ư¡
	// 1. Camera �� �����̴�
	// 2. Camera �� Z ���� �ٶ󺸰� �ִ�.
	Matrix		m_matView;
	Matrix		m_matViewInv;
	Matrix		m_matProj;		// ����
	Matrix		m_matProjInv;

	// Check �� Layer �� �� �� �ְ� (���� ó��)
	// �������� ���� ���迡�� �θ� Layer �� ���ٰ� �ص�
	// �ڽ� ������Ʈ�� Layer �� �ٸ� �� �ִ�.
	UINT		m_LayerCheck;
	tRay		m_Ray;

	//vector<CGameObject*>	m_vecDeferred; -> Instancing �߰��� ����ȭ
	// ��ü �з� ����ȭ
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
	void LayerCheckAll(UINT value) { m_LayerCheck = value; } // ��� ���ڿ� 1 �� �ִ�
	const UINT GetLayerCheck() { return m_LayerCheck; }

	void SortObject();
	void SortObject_ShadowMap();

public:
	virtual void Begin() override;
	virtual	void FinalTick() override; // �ݵ�� ����
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

