#include "pch.h"
#include "TileMapEditor.h"

#include "CImGuiManager.h"
#include "TileMapSubUI.h"

#include "TileView.h"
#include "TileMapDetail.h"
#include "AtlasView.h"

TileMapEditor::TileMapEditor()
	: EditorUI("TileMapEditor", "##TileMapEditor")
	, m_Sub{}
	, m_PrevActive{}
	, m_TileMap(nullptr)
{
	m_Sub[TILEVIEW] = CImGuiManager::GetInst()->FindEditorUI<TileView>("Tile View");
	m_Sub[ATLASVIEW] = CImGuiManager::GetInst()->FindEditorUI<AtlasView>("Atlas View");
	m_Sub[DETAIL] = CImGuiManager::GetInst()->FindEditorUI<TileMapDetail>("Detail");

	for (int i = 0; i < 3; i++)
	{
		m_Sub[i]->SetActive(true);
		m_Sub[i]->m_Owner = this;
	}
}

TileMapEditor::~TileMapEditor()
{

}

void TileMapEditor::SetTileMap(CTileMap* tileMapComponent)
{
	m_TileMap = tileMapComponent;

	for (int i = 0; i < 3; i++)
	{
		m_Sub[i]->SetTileMap(tileMapComponent);
	}
}

void TileMapEditor::Render_Tick()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File##TileMapEditor"))
		{

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View##TileMapEditor"))
		{
			bool IsActive[3] = { m_Sub[0]->IsActive(), m_Sub[1]->IsActive(), m_Sub[2]->IsActive() };

			if (ImGui::MenuItem("Tile View##TileMapEditor", nullptr, &IsActive[TILEVIEW]))
			{
				m_Sub[TILEVIEW]->SetActive(IsActive[TILEVIEW]);
			}

			if (ImGui::MenuItem("Atlas View##TileMapEditor", nullptr, &IsActive[ATLASVIEW]))
			{
				m_Sub[ATLASVIEW]->SetActive(IsActive[ATLASVIEW]);
			}

			if (ImGui::MenuItem("Detail##TileMapEditor", nullptr, &IsActive[DETAIL]))
			{
				m_Sub[DETAIL]->SetActive(IsActive[DETAIL]);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void TileMapEditor::Activate()
{
	for (int i = 0; i < 3; i++)
	{
		if(m_PrevActive[i])
			m_Sub[i]->SetActive(true);
	}
}

void TileMapEditor::Deactivate()
{
	for (int i = 0; i < 3; i++)
	{
		m_PrevActive[i] = m_Sub[i]->IsActive();
		m_Sub[i]->SetActive(false);
	}
}
