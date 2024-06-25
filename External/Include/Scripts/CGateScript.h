#pragma once

#include <Engine\CScript.h>

class CGateScript : public CScript
{
private:
	int				m_MonsterCount;
	CCollider2D*	m_GateCollider;
	bool			m_IsClear;

public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	void DeathMonsterCount() { m_MonsterCount--; }

public:
	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CGateScript);

public:
	CGateScript();
	~CGateScript();
};

