#include "pch.h"
#include "AtlasView.h"

AtlasView::AtlasView()
	: TileMapSubUI("Atlas View", "##AtlasView")
	, m_DefaultSize(200.f)
	, m_AtlasScale(1.f)
	, m_ImageIndex(-1)
{
}

AtlasView::~AtlasView()
{
}

void AtlasView::Render_Tick()
{
	CTileMap* pTileMap = GetTileMap();
	if (pTileMap == nullptr)
		return;

	Ptr<CTexture> pAtlas = pTileMap->GetAtlas();
	if (pAtlas == nullptr)
		return;

	// ���õ� Atlas Texture ���
	ImTextureID TexID = (nullptr == pAtlas) ? nullptr : pAtlas->GetSRV().Get();
	ImVec4 BorderColor = (nullptr == pAtlas) ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(1.f, 1.f, 1.f, 0.f);

	float AspectRatio = pAtlas->GetWidth() / pAtlas->GetHeight();

	ImGui::Image(TexID
		, ImVec2(m_DefaultSize * m_AtlasScale, (m_DefaultSize / AspectRatio) * m_AtlasScale)
		, ImVec2(0.0f, 0.0f)
		, ImVec2(1.0f, 1.0f)
		, ImGui::GetStyleColorVec4(ImGuiCol_Text)
		, BorderColor);

	// �ֱ� Widget �� Screen Pos
	m_ImageRectMin = Vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
	ImVec2 vMousePos = ImGui::GetMousePos();

	float rectMin[2] = { m_ImageRectMin.x, m_ImageRectMin.y };
	ImGui::InputFloat2("Rect Min", rectMin, 0);

	float mousePos[2] = { vMousePos.x, vMousePos.y };
	ImGui::InputFloat2("Mouse Pos", mousePos, 0);

	ImGui::InputInt("Image Index", &m_ImageIndex, 0, ImGuiInputTextFlags_ReadOnly);

	// ���콺 �ٷ� ���� �ܾƿ�
	if (ImGui::GetIO().MouseWheel > 0.f)
	{
		m_AtlasScale += 0.1f;

		if (m_AtlasScale > 3.0f)
			m_AtlasScale = 3.0f;
	}
	else if(ImGui::GetIO().MouseWheel < 0.f)
	{
		m_AtlasScale -= 0.1f;

		if (m_AtlasScale < 0.2f)
			m_AtlasScale = 0.2f;
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		SetUIMove(false);
		MouseCheck();
	}
	else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		SetUIMove(true);
	}


	if (m_ImageIndex != -1)
		DrawSelectedTile();
}

void AtlasView::MouseCheck()
{
	// ���� ���콺 ��ǥ��, �̹��� �»���� ���� ���� ���Ѵ�.
	Vec2 vMousePos = Vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	Vec2 vOffsetMousePos = vMousePos - m_ImageRectMin;

	// Tile Map �� �����ϴ� Atlas Texture �� ���� �ػ󵵿�,
	// Atlas View ���� ����ϴ� Texture ���� ���� ���� ���� ���Ѵ�.
	CTileMap* pTileMap = GetTileMap();
	Ptr<CTexture> pAtlas = pTileMap->GetAtlas();
	float Ratio = m_DefaultSize * m_AtlasScale / pAtlas->GetWidth();

	// Tile Map �� ���� �ػ󵵻󿡼��� Tile Size ��,
	// ������ ���� ������ ������Ѽ� ���콺 ���� ��ġ�� üũ�Ѵ�.
	Vec2 AtlasTileSize = pTileMap->GetAtlasTileSize();
	AtlasTileSize *= Ratio;

	int Col = (int)(vOffsetMousePos.x / (int)AtlasTileSize.x);
	int Row = (int)(vOffsetMousePos.y / (int)AtlasTileSize.y);

	// Atlas Texture �� �ܺθ� Ŭ���ϸ� �ƹ��ϵ� �� �Ͼ�� ����
	if (vOffsetMousePos.x < 0.f || pTileMap->GetAtlasMaxCol() <= Col
	 || vOffsetMousePos.y < 0.f || pTileMap->GetAtlasMaxRow() <= Row)
	{
		return;
	}

	// ���� Image Index ��ǥ
	m_ImageIndex = pTileMap->GetAtlasMaxCol() * Row + Col;
}

void AtlasView::DrawSelectedTile()
{
	// Tile Map �� �����ϴ� Atlas Texture �� ���� �ػ󵵿�,
	// Atlas View ���� ����ϴ� Texture ���� ���� ���� ���� ���Ѵ�.
	CTileMap* pTileMap = GetTileMap();
	Ptr<CTexture> pAtlas = pTileMap->GetAtlas();
	float Ratio = m_DefaultSize * m_AtlasScale / pAtlas->GetWidth();

	// Tile Map �� ���� �ػ󵵻󿡼��� Tile Size ��,
	// ������ ���� ������ ������Ѽ� ���콺 ���� ��ġ�� üũ�Ѵ�.
	Vec2 AtlasTileSize = pTileMap->GetAtlasTileSize();
	AtlasTileSize *= Ratio;

	int Row = m_ImageIndex / (int)pTileMap->GetAtlasMaxCol();
	int Col = m_ImageIndex % (int)pTileMap->GetAtlasMaxCol();

	ImVec2 LeftTop = ImVec2(Col * AtlasTileSize.x, Row * AtlasTileSize.y);
	LeftTop.x += m_ImageRectMin.x;
	LeftTop.y += m_ImageRectMin.y;

	ImVec2 RightBot = ImVec2(LeftTop.x + AtlasTileSize.x, LeftTop.y + AtlasTileSize.y);

	ImGui::GetWindowDrawList()->AddRect(LeftTop, RightBot
									, ImGui::GetColorU32(ImVec4(1.f, 1.f, 0.f, 1.f))
									, 1.f, 0, 2);
}

//ImVec2 vScreenPos	= ImGui::GetCursorScreenPos();
//ImVec2 vRectMin = ImGui::GetItemRectMin();
//ImVec2 vRectMax = ImGui::GetItemRectMax();
//ImVec2 vMousePos = ImGui::GetMousePos();