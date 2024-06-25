#pragma once
#include "ComponentUI.h"

class Light2DUI : public ComponentUI
{
private:

public:
	virtual void Render_Tick() override;

public:
	Light2DUI();
	~Light2DUI();
};

