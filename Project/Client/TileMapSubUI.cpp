#include "pch.h"
#include "TileMapSubUI.h"

#include "AtlasView.h"

TileMapSubUI::TileMapSubUI(const string& name, const  string& id)
	: EditorUI(name, id)
	, m_Owner(nullptr)
	, m_TileMap(nullptr)
{

}

TileMapSubUI::~TileMapSubUI()
{

}

void TileMapSubUI::SetTileMap(CTileMap* tileMap)
{
	if (m_TileMap)
	{
		m_TileMap->SetCapture(false);
	}

	m_TileMap = tileMap;

	if (m_TileMap)
	{
		m_TileMap->SetCapture(true);
	}
}

TileView* TileMapSubUI::GetTileView()
{
	return (TileView*)m_Owner->GetSubUI(TILEVIEW);
}

AtlasView* TileMapSubUI::GetAtlasView()
{
	return (AtlasView*)m_Owner->GetSubUI(ATLASVIEW);
}

TileMapDetail* TileMapSubUI::GetDetailView()
{
	return (TileMapDetail*)m_Owner->GetSubUI(DETAIL);
}
