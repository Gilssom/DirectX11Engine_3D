#pragma once
#include "ComponentUI.h"

class Animator3DUI : public ComponentUI
{
private:

public:
	virtual void Render_Tick() override;

public:
	Animator3DUI();
	~Animator3DUI();
};