#pragma once
#include "CRenderComponent.h"

class CDecal : public CRenderComponent
{
private:
	Ptr<CTexture>	m_OutputTex;
	bool			m_AsLight;	// 발광 여부
	// Color Target <-> Emissive Target 교체 활용

	float			m_LightPower;

public:
	void SetOutputTexture(Ptr<CTexture> tex) { m_OutputTex = tex; }
	Ptr<CTexture> GetOutputTexture() { return m_OutputTex; }

	void SetAsLight(bool light) { m_AsLight = light; }
	bool GetAsLight() { return m_AsLight; }
	void SetDecalPower(float power)
	{
		m_LightPower = power;

		if (m_LightPower < 0.f)
			m_LightPower = 0.f;
		else if (m_LightPower > 1.f)
			m_LightPower = 1.f;
	}
	float GetDecalPower() { return m_LightPower; }

public:
	virtual void FinalTick() override;
	virtual void Render() override;

public:
	CLONE(CDecal);

public:
	CDecal();
	~CDecal();
};