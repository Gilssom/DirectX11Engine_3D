#pragma once
#include "CComponent.h"

class CCamera : public CComponent
{
private:
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
	Matrix		m_matProj;		// 투영

	// Check 한 Layer 만 볼 수 있게 (필터 처리)
	// 오브젝츠 계층 관계에서 부모 Layer 를 본다고 해도
	// 자식 오브젝트는 Layer 가 다를 수 있다.
	UINT		m_LayerCheck;

	vector<CGameObject*>	m_vecOpaque;
	vector<CGameObject*>	m_vecMasked;
	vector<CGameObject*>	m_vecTransParent;
	vector<CGameObject*>	m_vecParticle;
	vector<CGameObject*>	m_vecPostProcess;

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

	void LayerCheck(int layerIdx);
	void LayerCheckAll(UINT value) { m_LayerCheck = value; } // 모든 숫자에 1 을 넣는
	const UINT GetLayerCheck() { return m_LayerCheck; }

	void SortObject();

public:
	virtual void Begin() override;
	virtual	void FinalTick() override; // 반드시 구현
	void Render();

private:
	void SortClear();

	void Render_opaque();
	void Render_masked();
	void Render_transparent();
	void Render_particle();
	void Render_postprocess();

public:
	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CCamera);

public:
	CCamera();
	CCamera(const CCamera& other);
	~CCamera();
};

