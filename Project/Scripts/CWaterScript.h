#pragma once
#include <Engine\CScript.h>

class CWaterScript : public CScript
{
private:
	Ptr<CTexture>	m_WaterNormalMap;		// Water Normal Map
	Vec3			m_CamPos;				// Camera Position
	float			m_FresnelPower;			// Fresnel Power
	float			m_NormalStrength;		// 물결 왜곡 강도

	float			m_WaveSpeed;			// 파도 속도
	float			m_WaveAmplitude;		// 파도 높이
	float			m_WaveFrequency;		// 파도 빈도 (주기)
	
public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	CLONE(CWaterScript);

public:
	CWaterScript();
	~CWaterScript();
};