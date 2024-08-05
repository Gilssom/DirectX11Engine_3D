#pragma once
#include "CComponent.h"

#include "ptr.h"
#include "CMesh.h"
#include "CMaterial.h"

class CMRT;

class CLight3D : public CComponent
{
// ������ ������ ���忡 �������� ���� �� �ִ�.
// �׷��� �ǽð����� ������ �޶����� �������� ���� ������ �Ѱ��־�� �Ҷ���
// ��� ���۷δ� ������ �ɸ���. (������������ ����)
// �׷��� ���ؼ� ���������� ������ ũ�⸦ �޸� �� �� �ִ� ����ȭ���۸� ����ؾ� ��.
private:
	tLightInfo		m_Info;
	int				m_LightIdx;

	Ptr<CMesh>		m_VolumeMesh;
	Ptr<CMaterial>	m_LightMaterial;
	
	CGameObject*	m_LightCamObject;	// ���� ���� Camera Object
	CMRT*			m_ShadowMapMRT;		// Shadow Map MRT

public:
	void SetLightType(LIGHT_TYPE type);

	void SetDiffuse(Vec3 diffuse) { m_Info.Light.vDiffuse = diffuse; }
	void SetAmbient(Vec3 ambient) { m_Info.Light.vAmbient = ambient; }
	void SetSpecular(Vec3 specular) { m_Info.Light.vMaxSpecular = specular; }

	void SetRange(float range);
	void SetAngle(float angle) { m_Info.Angle = angle; }

	const tLightInfo& GetLightInfo() { return m_Info; }

public:
	void Render_ShadowMap();
	void Lighting();

public:
	virtual void FinalTick() override;

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CLight3D);

public:
	CLight3D();
	CLight3D(const CLight3D& origin);
	~CLight3D();
};