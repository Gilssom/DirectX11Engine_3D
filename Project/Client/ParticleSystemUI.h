#pragma once
#include "ComponentUI.h"

class ParticleSystemUI : public ComponentUI
{
private:

public:
	virtual void Render_Tick() override;

public:
	ParticleSystemUI();
	~ParticleSystemUI();
};