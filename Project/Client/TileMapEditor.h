#pragma once
#include "EditorUI.h"

#include <Engine\CTileMap.h>

enum TILEMAP_SUB
{
	TILEVIEW,
	ATLASVIEW,
	DETAIL,
};

class TileMapEditor : public EditorUI
{
private:
	class TileMapSubUI*		m_Sub[3];
	bool					m_PrevActive[3];

	CTileMap*				m_TileMap;

public:
	void SetTileMap(CTileMap* tileMapComponent);
	TileMapSubUI* GetSubUI(TILEMAP_SUB sub) { return m_Sub[sub]; }

public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	TileMapEditor();
	~TileMapEditor();
};

