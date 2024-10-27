#pragma once
#include <Engine\\CScript.h>

struct TrailSegment
{
	Vec3 position;		// 검의 위치
	Vec3 rotation;		// 검의 회전
	float timeAlive;	// 해당 위치가 유지된 시간
};

class CAnimStateMachine;

class CPlayerScript : public CScript
{
private:
	CAnimStateMachine*	m_ASM;

	bool				m_ReadyAttack;
	bool				m_IsAttacking;
	bool				m_IsHit;
	bool				m_IsDeath;

	float				m_Speed;
	float				m_RotSpeed;

	bool				m_IsDashing = true;
	float				m_DashTime;
	float				m_MaxDashTime;
	float				m_InitialDashSpeed;
	float				m_MaxDashSpeed;

	bool				m_WeaponIsEmissive;
	float				m_WeaponEmissive;

	Ptr<CPrefab>		m_Test;
	CGameObject*		m_WeaponPos;
	CGameObject*		m_TrailEffect;

public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	void Move(bool isRun);

	void AttackStart();
	void Attack();
	void AttackEnd();

	void DashForward();

	void Hit();
	void HitEnd();

	void Death();
	void DeathEnd();

public:
	CLONE(CPlayerScript);

public:
	CPlayerScript();
	~CPlayerScript();
};