#pragma once
#include "AssetUI.h"

class ComputeShaderUI : public AssetUI
{
private:


public:
	virtual void Render_Tick() override;

public:
	ComputeShaderUI();
	~ComputeShaderUI();
};

