#pragma once
#include "CComputeShader.h"

class CParticleTick : public CComputeShader
{
private:



public:
	virtual int Binding() override;
	virtual void CalculateGroupNum() override;
	virtual void Clear() override;


public:
	CParticleTick();
	~CParticleTick();
};

