#pragma once
#include <Engine\\CScript.h>

class CAnimStateMachine;

class CPlayerScript : public CScript
{
private:
	CAnimStateMachine* m_ASM;

public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	void Move();
	void Attack();
	void AttackEnd();

public:
	CLONE(CPlayerScript);

public:
	CPlayerScript();
	~CPlayerScript();
};