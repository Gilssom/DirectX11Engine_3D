#pragma once
#include "CComponent.h"

class CLight2D : public CComponent
{
private:
	tLightInfo	m_Info;

	// 광원의 개수는 월드에 여러개가 있을 수 있다.
	// 그러면 실시간으로 개수가 달라지는 여러개의 광원 정보를 넘겨주어야 할때는
	// 상수 버퍼로는 제한이 걸린다. (가변적이지가 않음)
	// 그렇기 위해서 가변적으로 개수의 크기를 달리 할 수 있는 구조화버퍼를 사용해야 함.

	int			m_LightIdx;

public:
	void SetLightType(LIGHT_TYPE type);

	void SetDiffuse(Vec3 diffuse) { m_Info.Light.vDiffuse = diffuse; }
	void SetAmbient(Vec3 ambient) { m_Info.Light.vAmbient = ambient; }
	void SetSpecular(Vec3 specular) { m_Info.Light.vMaxSpecular = specular; }

	void SetRange(float range) { m_Info.Range = range; }
	void SetAngle(float angle) { m_Info.Angle = angle; }

	const tLightInfo& GetLightInfo() { return m_Info; }

public:
	virtual void FinalTick() override;

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CLight2D);

public:
	CLight2D();
	~CLight2D();
};

