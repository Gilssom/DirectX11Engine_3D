#pragma once
#include <Engine\\CScript.h>
#include "CScriptManager.h"

class CMonsterScript : public CScript
{
protected:
	CGameObject*	m_OriginPosObject;

	CGameObject*	m_Target;

	float			m_Speed;
	int				m_MaxHealth;
	int				m_Health;
	int				m_Armor;
	int				m_Damage;

	float			m_AttackRange;
	float			m_AttackCoolTime;

	bool			m_IsDeath;
	bool			m_IsAttack;

	float			m_TraceDistance;
	float			m_HitTimer;

	CAnimator2D*	m_Animator;

	int				m_RefentenceHit;

	vector<CGameObject*> m_DamageFonts;

public:
	int GetMaxHealth() { return m_MaxHealth; }
	int GetCurHealth() { return m_Health; }

public:
	void Attack(const wstring& animName);
	void TakeDamage(float minDamage, float maxDamage, const wstring& hitName = L"");
	void PlayAnimation(const wstring& animName, bool repeat);

	virtual void HitEvent() {};
	void HitEffect(const wstring& hitEventName);
	void KnockBack(CGameObject* target, const wstring& hitEventName);

	void DamageFont(int damage, bool critical);

public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void BeginOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) {};
	virtual void Overlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) {};
	virtual void EndOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) {};

	virtual void AnimationInit() {};

public:
	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CMonsterScript);

public:
	CMonsterScript(SCRIPT_TYPE type);
	CMonsterScript(SCRIPT_TYPE type, const CMonsterScript& other);
	~CMonsterScript();
};