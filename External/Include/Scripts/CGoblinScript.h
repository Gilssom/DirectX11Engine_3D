#pragma once
#include "CMonsterScript.h"

enum class eGoblinState
{
	Idle,
	Move,
	Patrol,
	Attack,
	Hit,
	Airborne,
	Death,
};

class CGoblinScript : public CMonsterScript
{
private:
	eGoblinState			m_CurrentState;
	eGoblinState			m_PrevState;
	float					m_StateTimer;
	float					m_ChaseCoolTime;
	float					m_CurChaseCoolTime;
	float					m_CurAttackCoolTime;
	bool					m_IsAttack;
	float					m_CurHitTime;

	bool					m_IsAirborne;
	float					m_AirborneDuration;
	float					m_AirbornePower;
	Vec3					m_AirborneVelocity;
	float					m_PrevPosY;

	int						m_PatrolDirection;
	float					m_ChangeDirectionCoolTime;
	Vec3					m_Direction;

	CGameObject*			m_Player;
	CGameObject*			m_CurGate;

	vector<CCollider2D*>	m_AttackArea;

public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void BeginOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;
	virtual void Overlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;
	virtual void EndOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;

	virtual void AnimationInit() override;

public:
	void SetState(eGoblinState nextState);
	eGoblinState GetState() { return m_CurrentState; }

private:
	void Idle();
	void Move();
	void Patrol();
	void Attack();
	void Hit();
	void Airborne();
	void Death();
	virtual void HitEvent() override;

	void UpdataDirection();
	bool IsPlayerTarget();

public:
	CLONE(CGoblinScript);

public:
	CGoblinScript();
	virtual ~CGoblinScript();
};