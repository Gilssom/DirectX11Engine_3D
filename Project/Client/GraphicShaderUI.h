#pragma once
#include "AssetUI.h"

class GraphicShaderUI : public AssetUI
{
private:


public:
	virtual void Render_Tick() override;

public:
	GraphicShaderUI();
	~GraphicShaderUI();
};

