#include "pch.h"
#include "TileMapDetail.h"

#include "ListUI.h"

TileMapDetail::TileMapDetail()
	: TileMapSubUI("Detail", "##TileMapDetail")
{
}

TileMapDetail::~TileMapDetail()
{
}

void TileMapDetail::Render_Tick()
{
	CTileMap* pTileMap = GetTileMap();

	if (pTileMap == nullptr)
		return;

	// Tile Row - Col
	int row_col[2] = { pTileMap->GetRow(), pTileMap->GetCol() };
	ImGui::Text("Row"); SAME_SET(100); 	
	ImGui::SetNextItemWidth(50);
	ImGui::InputInt("##TileMapRow", &row_col[0], 0);

	ImGui::Text("Col"); SAME_SET(100);
	ImGui::SetNextItemWidth(50);
	ImGui::InputInt("##TileMapCol", &row_col[1], 0);

	// Tile Each Size
	Vec2 vTileSize = pTileMap->GetTileSize();
	float TileSize[2] = { vTileSize.x, vTileSize.y };

	ImGui::Text("Tile Size"); SAME_SET(100);
	ImGui::InputFloat2("##TileEachSize", TileSize);

	// Atlas Texture
	ImGui::SeparatorText("Atlas Texture Setting");
	Ptr<CTexture> pAtlas = pTileMap->GetAtlas();
	string AtlasName;
	if (pAtlas != nullptr)
	{
		AtlasName = ToString(pAtlas->GetKey());
	}

	ImGui::Text("Atlas"); SAME_SET(100);
	ImGui::SetNextItemWidth(200);
	ImGui::InputText("##TileMapDetail_Atlas", (char*)AtlasName.c_str(), AtlasName.capacity(), ImGuiInputTextFlags_ReadOnly);
	
	//SAME_SET(100);

	// Atlas Texture Change Button 추가
	if (ImGui::Button("Atlas Change##TileMapDetail_AtlasBtn"))
	{
		// ListUI 를 활성화한다
		ListUI* pListUI = CImGuiManager::GetInst()->FindEditorUI<ListUI>("List");

		if (pListUI != nullptr && !pListUI->IsActive())
		{
			pListUI->SetModal(true);
			pListUI->SetActive(true);
			pListUI->SetFocus();

			vector<string> vecNames;
			CAssetManager::GetInst()->GetAssetNames(ASSET_TYPE::TEXTURE, vecNames);
			pListUI->AddItem(vecNames);

			pListUI->RegisterDoubleClickDelegate(this, (UI_DELEGATE1)&TileMapDetail::SelectTexture);
		}
	}

	// Atlas Texture 출력
	ImTextureID TexID = (nullptr == pAtlas) ? nullptr : pAtlas->GetSRV().Get();
	ImVec4 BorderColor = (nullptr == pAtlas) ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(1.f, 1.f, 1.f, 0.f);

	ImGui::Image(TexID
		, ImVec2(80.f, 80.f)
		, ImVec2(0.0f, 0.0f)
		, ImVec2(1.0f, 1.0f)
		, ImGui::GetStyleColorVec4(ImGuiCol_Text)
		, BorderColor);

	// Atlas Tile Size
	Vec2 vAtlasSize = pTileMap->GetAtlasTileSize();
	TileSize[0] = vAtlasSize.x;
	TileSize[1] = vAtlasSize.y;

	ImGui::Text("Atlas Tile Size"); SAME_SET(130);
	ImGui::SetNextItemWidth(150);
	if (ImGui::InputFloat2("##AtlasTileEachSize", TileSize))
	{
		pTileMap->SetAtlasTileSize(Vec2(TileSize[0], TileSize[1]));
	}

	// Read Only Information
	// Atlas Max Row - Col
	row_col[0] = pTileMap->GetAtlasMaxRow();
	row_col[1] = pTileMap->GetAtlasMaxCol();

	ImGui::Text("Atlas Max Row"); SAME_SET(130);
	ImGui::SetNextItemWidth(150);
	ImGui::InputInt("##AtlaxMaxRow", &row_col[0], 0, ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Atlas Max Col"); SAME_SET(130);
	ImGui::SetNextItemWidth(150);
	ImGui::InputInt("##AtlasMaxCol", &row_col[1], 0, ImGuiInputTextFlags_ReadOnly);

	// Atlas Resolution
	Vec2 vResolution = nullptr == pAtlas ? Vec2(0.f, 0.f) : Vec2(pAtlas->GetWidth(), pAtlas->GetHeight());
	float Resolution[2] = { vResolution.x, vResolution.y };

	ImGui::Text("Atlas Resolution"); SAME_SET(130); 	
	ImGui::SetNextItemWidth(150);
	ImGui::InputFloat2("##AtlaxResolution", Resolution, "%.f", ImGuiInputTextFlags_ReadOnly);
}

UINT TileMapDetail::SelectTexture(DWORD_PTR selected)
{
	string* pStr = (string*)selected;

	wstring strTexKey = ToWString(*pStr);

	Ptr<CTexture> pTexture = CAssetManager::GetInst()->FindAsset<CTexture>(strTexKey);

	assert(GetTileMap() != nullptr && pTexture != nullptr);

	GetTileMap()->SetAtlasTexture(pTexture);

	SetFocus();

	return 0;
}
