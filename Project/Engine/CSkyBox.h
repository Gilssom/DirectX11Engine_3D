#pragma once
#include "CRenderComponent.h"

enum SKYBOX_TYPE
{
	SPHERE,
	CUBE,
};

class CSkyBox : public CRenderComponent
{
private:
	SKYBOX_TYPE		m_Type;
	Ptr<CTexture>	m_SkyBoxTexture;

public:
	void SetSkyBoxType(SKYBOX_TYPE type);
	SKYBOX_TYPE GetSkyBoxType() { return m_Type; }

	void SetSkyBoxTexture(Ptr<CTexture> texture) { m_SkyBoxTexture = texture; }
	Ptr<CTexture> GetSkyBoxTexture() { return m_SkyBoxTexture; }

public:
	virtual void FinalTick() override;
	virtual void Render() override;

public:
	CLONE(CSkyBox);

public:
	CSkyBox();
	~CSkyBox();
};