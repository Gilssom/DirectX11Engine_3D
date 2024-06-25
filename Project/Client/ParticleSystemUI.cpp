#include "pch.h"
#include "ParticleSystemUI.h"

ParticleSystemUI::ParticleSystemUI()
	: ComponentUI("ParticleSystem", "##ParticleSystemUI", COMPONENT_TYPE::PARTICLESYSTEM)
{
	SIZE;
}

ParticleSystemUI::~ParticleSystemUI()
{
}

void ParticleSystemUI::Render_Tick()
{
	Render_Title();
}