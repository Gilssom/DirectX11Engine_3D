#pragma once
#include "AssetUI.h"

class SoundUI : public AssetUI
{
private:


public:
	virtual void Render_Tick() override;

public:
	SoundUI();
	~SoundUI();
};

