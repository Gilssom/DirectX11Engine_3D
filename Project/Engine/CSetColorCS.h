#pragma once
#include "CComputeShader.h"

#include "CTexture.h"

class CSetColorCS : public CComputeShader
{
private:
	Ptr<CTexture>	m_TargetTex;
	Vec4			m_ClearColor;

public:
	void SetTargetTextrue(Ptr<CTexture> tex) { m_TargetTex = tex; }
	void SetClearColor(Vec3 color) { m_ClearColor = color; m_ClearColor.w = 1.f; }


public:
	virtual int Binding() override;
	virtual void CalculateGroupNum() override;
	virtual void Clear() override;


public:
	CSetColorCS();
	~CSetColorCS();
};

