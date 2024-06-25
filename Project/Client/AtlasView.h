#pragma once
#include "TileMapSubUI.h"

class AtlasView : public TileMapSubUI
{
private:
	float	m_DefaultSize;
	float	m_AtlasScale;
	
	Vec2	m_ImageRectMin;
	int		m_ImageIndex;

public:
	int GetImageIndex() { return m_ImageIndex; }

public:
	virtual void Render_Tick() override;

private:
	void MouseCheck();
	void DrawSelectedTile();

public:
	AtlasView();
	~AtlasView();
};

