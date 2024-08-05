#pragma once
#include "CComponent.h"

#include "ptr.h"
#include "CMesh.h"
#include "CMaterial.h"

class CMRT;

class CLight3D : public CComponent
{
// 광원의 개수는 월드에 여러개가 있을 수 있다.
// 그러면 실시간으로 개수가 달라지는 여러개의 광원 정보를 넘겨주어야 할때는
// 상수 버퍼로는 제한이 걸린다. (가변적이지가 않음)
// 그렇기 위해서 가변적으로 개수의 크기를 달리 할 수 있는 구조화버퍼를 사용해야 함.
private:
	tLightInfo		m_Info;
	int				m_LightIdx;

	Ptr<CMesh>		m_VolumeMesh;
	Ptr<CMaterial>	m_LightMaterial;
	
	CGameObject*	m_LightCamObject;	// 광원 전용 Camera Object
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