#pragma once
#include <Engine\\CScript.h>

class CBackGroundScript : public CScript
{
private:
	float m_Intence;


public:
	virtual void Tick() override;


public:
	CLONE(CBackGroundScript);


public:
	CBackGroundScript();
	~CBackGroundScript();
};

