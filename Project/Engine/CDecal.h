#pragma once
#include "CRenderComponent.h"

class CDecal : public CRenderComponent
{
private:
	Ptr<CTexture>	m_OutputTex;
	float			m_Alpha;

	bool			m_AsEmissive;	// 발광 여부
	// Color Target <-> Emissive Target 교체 활용

public:
	void SetOutputTexture(Ptr<CTexture> tex) { m_OutputTex = tex; }

public:
	virtual void FinalTick() override;
	virtual void Render() override;

public:
	CLONE(CDecal);

public:
	CDecal();
	~CDecal();
};