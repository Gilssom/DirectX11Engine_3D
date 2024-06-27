#pragma once
#include "ComponentUI.h"
class Light3DUI : public ComponentUI
{
public:
	virtual void Render_Tick() override;

public:
	Light3DUI();
	~Light3DUI();
};

