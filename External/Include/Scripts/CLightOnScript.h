#pragma once
#include <Engine\\CScript.h>

class CLightOnScript : public CScript
{
public:
	Vec4 m_Diffuse;
	float m_Duration;


public:
	virtual void Tick() override;


public:
	CLONE(CLightOnScript);


public:
	CLightOnScript();
	~CLightOnScript();
};

