#pragma once
#include "ComponentUI.h"

class DecalUI : public ComponentUI
{
private:


public:
	virtual void Render_Tick() override;

public:
	DecalUI();
	~DecalUI();
};