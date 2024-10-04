#pragma once
#include <Engine\\CScript.h>

class CAnimStateMachine;

class CPlayerScript : public CScript
{
private:
	CAnimStateMachine*	m_ASM;

	bool				m_ReadyAttack;
	bool				m_IsAttacking;
	bool				m_IsHit;
	bool				m_IsDeath;

public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	void Move();

	void AttackStart();
	void Attack();
	void AttackEnd();

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