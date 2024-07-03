#pragma once
#include "ComponentUI.h"

class SkyBoxUI : public ComponentUI
{
private:


public:
	virtual void Render_Tick() override;

public:
	SkyBoxUI();
	~SkyBoxUI();
};

