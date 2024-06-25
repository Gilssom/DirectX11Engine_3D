#pragma once
#include <Engine\\CScript.h>

#include "CPlayerSkillScript.h"
#include "CMonsterScript.h"

class CStructuredBuffer;

class CPlayerScript : public CScript
{
private:
	CGameObject*	m_OriginPosObject;

	int				m_MaxHp;
	int				m_CurHp;
	int				m_MaxMp;
	int				m_CurMp;
	int				m_Damage;

	float			m_Speed;
	CMonsterScript*	m_TargetMonster;

	bool			m_MoveLeft;
	bool			m_MoveRight;

	bool			m_CanMoveLeft, m_CanMoveRight, m_CanMoveUp, m_CanMoveDown;

	bool			m_IsJumping;
	float			m_JumpSpeed;
	float			m_JumpHeight;
	float			m_Gravity;
	float			m_PrevPosY;

	bool			m_CanKnockBack;
	float			m_KnockBackCoolTime;
	float			m_KnockBackCurTime;

	bool			m_HitDamage;
	Vec3			m_KnockBackVelocity;
	float			m_KnockBackPower;
	float			m_HitDownTime;
	float			m_CurHitDownTime;

	bool			m_IsFightLevel;
	bool			m_IsSkill;

	bool			m_IsCombo;
	int				m_ComboIndex;
	float			m_ComboTimer;
	const float		m_MaxComboTime;

	float			m_HitTimer;
	float			m_NaturalHealHp;
	float			m_NaturalHealMp;

	float			m_ResetTargetTime;
	float			m_SetTargetTime;

	vector<CCollider2D*> m_AttackArea;

public:
	void SetTarget(CMonsterScript* target);
	CMonsterScript* GetTarget() { return m_TargetMonster; }
	void ResetTarget() { m_TargetMonster = nullptr; }

	bool GetMoveLeft() { return m_MoveLeft; }
	bool GetMoveRight() { return m_MoveRight; }
	bool GetMove() { if (m_CanMoveLeft && m_CanMoveRight && m_CanMoveUp && m_CanMoveDown) return true; else return false; }
	CGameObject* GetOriginPos() { return m_OriginPosObject; }
	CCollider2D* GetAttackArea(int index) { return m_AttackArea[index]; }

	int GetMaxHp() { return m_MaxHp; }
	int GetCurHp() { return m_CurHp; }
	int GetMaxMp() { return m_MaxMp; }
	int GetCurMp() { return m_CurMp; }
	int GetDamage() { return m_Damage; }

public:
	void AnimationInit();
	void Move();
	void Jump();
	void Skill();

	void ExecuteComboSkill(int comboIndex);
	void ExecuteSkill(eSkillType skillType);

	void AttackCombo(int comboIndex);
	void ComboCheck();
	void TakeDamage(int damage);
	void KnockBack();
	void CoolTimeSound();

public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void BeginOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;
	virtual void Overlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;
	virtual void EndOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;

public:
	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CPlayerScript);

public:
	CPlayerScript();
	~CPlayerScript();
};