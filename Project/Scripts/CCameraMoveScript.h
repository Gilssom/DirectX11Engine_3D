#pragma once
#include <Engine\\CScript.h>

class CCameraMoveScript : public CScript
{
private:
	CGameObject*	m_TargetObject;
	float			m_Speed;

	Vec3 m_OriginalPos;			// 카메라의 원래 위치
	bool m_IsShaking;			// 카메라 쉐이크 여부
	float m_ShakeDuration;		// 쉐이크 지속 시간
	float m_ShakeTime;			// 경과된 시간
	float m_ShakeIntensity;		// 쉐이크 강도

public:
	virtual void Begin() override;
	virtual void Tick() override;

	void StartCameraShake(float intensity, float duration);
	void UpdateCameraShake();

private:
	void MoveByPerspective();
	void MoveByOrthographic();

public:
	void SetTargetObject(CGameObject* targetObject) { m_TargetObject = targetObject; }

public:
	CLONE(CCameraMoveScript);

public:
	CCameraMoveScript();
	~CCameraMoveScript();
};

