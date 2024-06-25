#pragma once
#include "EditorUI.h"

#include "TileMapEditor.h"
#include <Engine\CTileMap.h>

class TileMapSubUI : public EditorUI
{
private:
	TileMapEditor*		m_Owner;
	CTileMap*			m_TileMap;

public:
	void SetTileMap(CTileMap* tileMap);
	CTileMap* GetTileMap() { return m_TileMap; }

	TileMapEditor* GetOwner() { return m_Owner; }

	class TileView* GetTileView();
	class AtlasView* GetAtlasView();
	class TileMapDetail* GetDetailView();

public:
	TileMapSubUI(const string& name, const  string& id);
	~TileMapSubUI();


	friend class TileMapEditor;
};

