#pragma once
#include <Engine\CScript.h>

class CDamageScript : public CScript
{
private:
	float m_CurTime;
	float m_LiftTime;
	float m_Alpha;

public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	CLONE(CDamageScript);

public:
	CDamageScript();
	~CDamageScript();
};