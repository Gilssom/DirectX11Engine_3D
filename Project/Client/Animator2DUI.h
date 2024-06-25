#pragma once
#include "ComponentUI.h"

class Animator2DUI : public ComponentUI
{
private:

public:
	virtual void Render_Tick() override;

public:
	Animator2DUI();
	~Animator2DUI();
};

