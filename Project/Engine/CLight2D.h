#pragma once
#include "CComponent.h"

class CLight2D : public CComponent
{
private:
	tLightInfo	m_Info;

	// ������ ������ ���忡 �������� ���� �� �ִ�.
	// �׷��� �ǽð����� ������ �޶����� �������� ���� ������ �Ѱ��־�� �Ҷ���
	// ��� ���۷δ� ������ �ɸ���. (������������ ����)
	// �׷��� ���ؼ� ���������� ������ ũ�⸦ �޸� �� �� �ִ� ����ȭ���۸� ����ؾ� ��.

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

