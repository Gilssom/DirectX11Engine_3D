#pragma once
#include "AssetUI.h"

class PrefabUI : public AssetUI
{
private:


public:
	virtual void Render_Tick() override;

public:
	PrefabUI();
	~PrefabUI();
};

