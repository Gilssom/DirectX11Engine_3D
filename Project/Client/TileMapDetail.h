#pragma once
#include "TileMapSubUI.h"

class TileMapDetail : public TileMapSubUI
{
private:

public:
	virtual void Render_Tick() override;

public:
	UINT SelectTexture(DWORD_PTR selected);

public:
	TileMapDetail();
	~TileMapDetail();
};

