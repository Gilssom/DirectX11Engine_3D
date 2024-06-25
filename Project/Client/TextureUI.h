#pragma once
#include "AssetUI.h"

class TextureUI : public AssetUI
{
private:


public:
	virtual void Render_Tick() override;

public:
	TextureUI();
	~TextureUI();
};

