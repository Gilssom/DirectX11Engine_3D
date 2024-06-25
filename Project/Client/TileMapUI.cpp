#include "pch.h"
#include "TileMapUI.h"

#include "CImGuiManager.h"
#include "TileMapEditor.h"

TileMapUI::TileMapUI()
	: ComponentUI("TileMap", "##TileMapUI", COMPONENT_TYPE::TILEMAP)
{
	SIZE;
}

TileMapUI::~TileMapUI()
{

}

void TileMapUI::Render_Tick()
{
	Render_Title();

	ImGui::Text("Tile Map Editor");

	if (ImGui::Button("Open Editor##OpenTileMapEditor"))
	{
		TileMapEditor* pEditor = CImGuiManager::GetInst()->FindEditorUI<TileMapEditor>("TileMapEditor");
		pEditor->SetTileMap(GetTarget()->TileMap());
		pEditor->SetActive(true);
	}
}