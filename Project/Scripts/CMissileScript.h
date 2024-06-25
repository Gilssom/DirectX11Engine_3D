#pragma once
#include <Engine\\CScript.h>

class CMissileScript : public CScript
{
private:
	float m_Speed;


public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	CLONE(CMissileScript);

public:
	CMissileScript();
	~CMissileScript();
};