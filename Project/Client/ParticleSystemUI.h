#pragma once
#include "ComponentUI.h"

class ParticleSystemUI : public ComponentUI
{
private:

public:
	virtual void Render_Tick() override;

private:
	void SaveParticle(CGameObject* pParticleObject, const std::string& prefabName, const std::string& filePath);

public:
	ParticleSystemUI();
	~ParticleSystemUI();
};