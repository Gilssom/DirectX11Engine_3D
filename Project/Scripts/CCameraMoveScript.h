#pragma once
#include <Engine\\CScript.h>

class CCameraMoveScript : public CScript
{
private:
	CGameObject*	m_TargetObject;
	float			m_Speed;

	Vec3 m_OriginalPos;			// ī�޶��� ���� ��ġ
	bool m_IsShaking;			// ī�޶� ����ũ ����
	float m_ShakeDuration;		// ����ũ ���� �ð�
	float m_ShakeTime;			// ����� �ð�
	float m_ShakeIntensity;		// ����ũ ����

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

