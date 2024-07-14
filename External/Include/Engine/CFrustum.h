#pragma once
#include "CEntity.h"

// 절두체 컬링 평면 구성 요소
enum FACE_TYPE
{
	FACE_NEAR,
	FACE_FAR,
	FACE_TOP,
	FACE_BOT,
	FACE_LEFT,
	FACE_RIGHT,
};

class CFrustum : public CEntity
{
private:
	class CCamera*	m_Camera;
	Vec3			m_arrProjPos[8];
	Vec4			m_arrFace[6];

public:
	void SetOwner(CCamera* owner) { m_Camera = owner; }
	CCamera* GetOwner() { return m_Camera; }

public:
	void FinalTick();

public:
	CLONE(CFrustum);

public:
	CFrustum(CCamera* cam);
	~CFrustum();
};

