#pragma once
#include <Engine\\CScript.h>

class CPlayerScript : public CScript
{
public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	void Move();
	void Attack();

public:
	CLONE(CPlayerScript);

public:
	CPlayerScript();
	~CPlayerScript();
};