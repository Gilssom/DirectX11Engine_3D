#pragma once
#include "CMonsterScript.h"
#include <random>

enum class eBossState
{
	Idle,
	Move,
	Patrol,
	Attack,
	Rush,
	Buff,
	Death,
};

class CBossScript : public CMonsterScript
{
private:
	eBossState				m_CurrentState;
	eBossState				m_PrevState;
	float					m_StateTimer;
	float					m_ChaseCoolTime;
	float					m_CurChaseCoolTime;
	float					m_CurAttackCoolTime;

	float					m_RushDistance;
	float					m_RushCoolTime;
	Vec3					m_RushTargetPos;
	float					m_BuffCoolTime;

	bool					m_IsAttack;

	int						m_PatrolDirection;
	float					m_ChangeDirectionCoolTime;

	CGameObject* m_Player;

	vector<CCollider2D*>	m_AttackArea;

public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void BeginOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;
	virtual void Overlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;
	virtual void EndOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;

	virtual void AnimationInit() override;

public:
	void SetState(eBossState nextState);
	eBossState GetState() { return m_CurrentState; }

private:
	void Idle();
	void Move();
	void Patrol();
	void Attack();
	void Rush();
	void Buff();
	virtual void HitEvent() override;
	void Death();

	void UpdataDirection();
	bool IsPlayerTarget();

public:
	CLONE(CBossScript);

public:
	CBossScript();
	virtual ~CBossScript();
};