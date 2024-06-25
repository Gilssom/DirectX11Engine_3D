#include "pch.h"
#include "TileView.h"

#include "AtlasView.h"

TileView::TileView()
	: TileMapSubUI("Tile View", "##TileView")
	, m_DefaultSize(500.f)
	, m_CaptureScale(1.f)
	, m_ImageRectMin {}
	, m_SelectedCol(0)
	, m_SelectedRow(0)
{

}

TileView::~TileView()
{

}

void TileView::Render_Tick()
{
	CTileMap* pTileMap = GetTileMap();
	if (pTileMap == nullptr)
		return;

	Ptr<CTexture> pCaptureTex = pTileMap->GetCaptureTexture();
	
	// 선택된 Atlas Texture 출력
	ImTextureID TexID = (nullptr == pCaptureTex) ? nullptr : pCaptureTex->GetSRV().Get();
	ImVec4 BorderColor = (nullptr == pCaptureTex) ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(1.f, 1.f, 1.f, 0.f);

	// Tile Map 의 비율
	float AspectRatio = pTileMap->GetCol() / pTileMap->GetRow();

	// Image 출력
	ImGui::Image(TexID
		, ImVec2(m_DefaultSize * m_CaptureScale, (m_DefaultSize / AspectRatio) * m_CaptureScale)
		, ImVec2(0.0f, 0.0f)
		, ImVec2(1.0f, 1.0f)
		, ImGui::GetStyleColorVec4(ImGuiCol_Text)
		, BorderColor);

	// 최근 Widget 의 Screen Pos
	m_ImageRectMin = Vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);

	// 마우스 휠로 줌인 줌아웃
	if (ImGui::GetIO().MouseWheel > 0.f)
	{
		m_CaptureScale += 0.1f;

		if (m_CaptureScale > 3.0f)
			m_CaptureScale = 3.0f;
	}
	else if (ImGui::GetIO().MouseWheel < 0.f)
	{
		m_CaptureScale -= 0.1f;

		if (m_CaptureScale < 0.2f)
			m_CaptureScale = 0.2f;
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		SetUIMove(false);
		MouseCheck();
	}

	int RowCol[2] = { m_SelectedRow, m_SelectedCol };
	ImGui::InputInt2("Selected Row Col", RowCol, 0);
}

void TileView::Deactivate()
{
	CTileMap* pTileMap = GetTileMap();

	if(pTileMap != nullptr)
		pTileMap->SetCapture(false);
}

void TileView::MouseCheck()
{
	CTileMap* pTileMap = GetTileMap();
	if (pTileMap->GetCaptureTexture() == nullptr)
		return;

	// 현재 마우스 좌표와, 이미지 좌상단의 차이 값을 구한다.
	Vec2 vMousePos = Vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	Vec2 vOffsetMousePos = vMousePos - m_ImageRectMin;

	// Tile Map 의 실제 크기와 Capture Texture 간의 비율 차이를 구한다.
	Ptr<CTexture> pCaptureTex = pTileMap->GetCaptureTexture();

	Vec2 TileSize = pTileMap->GetTileSize();
	TileSize *= Vec2(pTileMap->GetCol(), pTileMap->GetRow());
	Vec2 CaptureTexResolution = Vec2(pCaptureTex->GetWidth(), pCaptureTex->GetHeight());

	// 1. 원본 타일이 Capture Texture 로 Rendering 될 때 변경된 비율
	Vec2 Ratio = CaptureTexResolution / TileSize;

	// 2. Capture Texture 를 ImGui::Image 로 출력할 때 변경된 비율
	float AspectRatio = pTileMap->GetCol() / pTileMap->GetRow();
	Ratio *= Vec2((m_DefaultSize * m_CaptureScale) / CaptureTexResolution.x,
			     ((m_DefaultSize * m_CaptureScale) / AspectRatio) / CaptureTexResolution.y);

	Vec2 TileEachSize = pTileMap->GetTileSize() * Ratio;

	m_SelectedCol = (int)(vOffsetMousePos.x / TileEachSize.x);
	m_SelectedRow = (int)(vOffsetMousePos.y / TileEachSize.y);
	
	// Atlas Texture 의 외부를 클릭하며 아무일도 안 일어나게 설정
	if (vOffsetMousePos.x < 0.f || pTileMap->GetCol() <= m_SelectedCol
		|| vOffsetMousePos.y < 0.f || pTileMap->GetRow() <= m_SelectedRow)
	{
		return;
	}

	// 이후 해당 Tile 교체 및 제작
	int imageIndex = GetAtlasView()->GetImageIndex();

	if (imageIndex == -1)
	{
		return;
	}

	pTileMap->SetImageIndex(m_SelectedRow, m_SelectedCol, imageIndex);
}