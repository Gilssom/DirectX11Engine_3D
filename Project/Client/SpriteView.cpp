#include "pch.h"
#include "SpriteView.h"

#include "AnimPreView.h"
#include "AnimDetail.h"

SpriteView::SpriteView()
	: AnimSubUI("Sprite View", "##Sprite View")
	, m_DefaultSize(500.f)
	, m_SpriteScale(1.f)
{
}

SpriteView::~SpriteView()
{

}

void SpriteView::Render_Tick()
{
	if (m_SpriteTexture == nullptr)
		return;

	ImTextureID TexID = (nullptr == m_SpriteTexture) ? nullptr : m_SpriteTexture->GetSRV().Get();
	ImVec4 BorderColor = (nullptr == m_SpriteTexture) ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(1.f, 1.f, 1.f, 0.f);

	float AspectRatio = m_SpriteTexture->GetWidth() / m_SpriteTexture->GetHeight();
	ImVec2 windowSize = ImGui::GetWindowSize();

	// 마우스 휠로 줌인 줌아웃
	if (ImGui::GetIO().MouseWheel > 0.f)
	{
		m_SpriteScale += 0.5f;

		if (m_SpriteScale > 10.0f)
			m_SpriteScale = 10.0f;
	}
	else if (ImGui::GetIO().MouseWheel < 0.f)
	{
		m_SpriteScale -= 0.5f;

		if (m_SpriteScale < 0.2f)
			m_SpriteScale = 0.2f;
	}

    // 이미지 크기
    m_ImageSize = ImVec2(m_DefaultSize * m_SpriteScale, (m_DefaultSize / AspectRatio) * m_SpriteScale);

    // 스크롤 최대 제한
    m_MaxScrollX = m_ImageSize.x - windowSize.x;
    m_MaxScrollY = m_ImageSize.y - windowSize.y;

    // 현재 커서 위치
    ImVec2 cursorPos = ImGui::GetCursorPos();

    // 가로 스크롤바
    if (m_MaxScrollX > 0)
    {
        ImGui::SetCursorPosY(windowSize.y - ImGui::GetFrameHeight()); // Position scrollbar at bottom
        ImGui::PushItemWidth(windowSize.x - ImGui::GetFrameHeight());
        ImGui::SliderFloat("##HorizontalScroll", &m_ScrollX, 0.0f, m_MaxScrollX, "");
        ImGui::PopItemWidth();
    }

    // 세로 스크롤바
    if (m_MaxScrollY > 0)
    {
        ImGui::SetCursorPosX(windowSize.x - ImGui::GetFrameHeight()); // Position scrollbar at right
        ImGui::VSliderFloat("##VerticalScroll", ImVec2(ImGui::GetFrameHeight(), windowSize.y - ImGui::GetFrameHeight()), &m_ScrollY, 0.0f, m_MaxScrollY, "");
    }

    // 커서 위치 설정
    ImGui::SetCursorPos(cursorPos);

    // 스크롤바에 의한 출력 이미지 좌표 설정
    m_Offset = ImVec2(-m_ScrollX, -m_ScrollY);
    ImGui::SetCursorPos(ImVec2(m_Offset.x + 30.f, m_Offset.y + 30.f));

    ImGui::Image(TexID
        , m_ImageSize
        , ImVec2(0.0f, 0.0f)
        , ImVec2(1.0f, 1.0f)
        , ImGui::GetStyleColorVec4(ImGuiCol_Text)
        , BorderColor);

    ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(128, 128, 128, 255));

    // 이미지 좌상단 위치
    m_ImageRectMin = Vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    DrawFrame(drawList);
    MouseCheck();
    SetOffset();

    // 이전 Frame 삭제 (Ctrl + Z)
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)) && ImGui::GetIO().KeyCtrl)
        DeleteFrame();

    // AnimPreView 에 Sprite View 넘기기
    AnimPreView* pPreView = GetAnimPreView();
    pPreView->SetSpriteView(this);

    AnimDetail* pDetail = GetAnimDetail();
    pDetail->SetSpriteView(this);

    isSelectFrame = false;
}

void SpriteView::Activate()
{

}

void SpriteView::Deactivate()
{
    m_SpriteScale = 1.f;
    m_SpriteTexture = nullptr;

    m_Frames.clear();
    m_SelectedFrameIndex = -1;

    m_ScrollX = 0.f;
    m_ScrollY = 0.f;
}

void SpriteView::MouseCheck()
{
    if (ImGui::IsItemHovered())
    {
        Vec2 vMousePos = Vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        m_MouseFinalPos = vMousePos - m_ImageRectMin;
        float Mag = m_SpriteTexture->GetWidth() / m_ImageSize.x;
        m_MouseFinalPos *= Mag;

        if (isSelectFrame)
            return;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            ImVec2 vStartPos;
            vStartPos.x = m_MouseFinalPos.x;
            vStartPos.y = m_MouseFinalPos.y;

            m_UIStartPos = ImGui::GetMousePos();
            m_StartPos = vStartPos;
            isSelecting = true;
        }

        if (isSelecting && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            ImVec2 vEndPos;
            vEndPos.x = m_MouseFinalPos.x;
            vEndPos.y = m_MouseFinalPos.y;

            m_UIEndPos = ImGui::GetMousePos();
            m_EndPos = vEndPos;
            isSelecting = false;
            AddFrame(m_StartPos, m_EndPos, m_UIStartPos, m_UIEndPos);
        }
    }

    if (isSelecting)
    {
        ImGui::GetWindowDrawList()->AddRect(m_UIStartPos, ImGui::GetMousePos(), IM_COL32(255, 0, 0, 255));
    }
}

void SpriteView::AddFrame(const ImVec2& topLeft, const ImVec2& bottomRight, const ImVec2& ui_TopLeft, const ImVec2& ui_BottomRight)
{
    Frame newFrame = { topLeft, bottomRight, ImVec2(0, 0), ui_TopLeft, ui_BottomRight };
    m_Frames.push_back(newFrame);
}

void SpriteView::DrawFrame(ImDrawList* drawList)
{
    for (int i = 0; i < m_Frames.size(); ++i) 
    {
        Frame& frame = m_Frames[i];
        drawList->AddRect(ImVec2(frame.UI_topLeft.x, frame.UI_topLeft.y)
                        , ImVec2(frame.UI_bottomRight.x, frame.UI_bottomRight.y)
                        , IM_COL32(0, 255, 0, 255));

        if (m_MouseFinalPos.x >= frame.topLeft.x && m_MouseFinalPos.x <= frame.bottomRight.x
            && m_MouseFinalPos.y >= frame.topLeft.y && m_MouseFinalPos.y <= frame.bottomRight.y)
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_SelectedFrameIndex = i;
                isSelectFrame = true;
            }
        }
    }
}

void SpriteView::SetOffset()
{
    // 오프셋 조정 UI
    if (m_SelectedFrameIndex != -1)
    {
        Frame& selectedFrame = m_Frames[m_SelectedFrameIndex];
        ImGui::Begin("Frame Editor");
        ImGui::Text("Adjust Offset");
        ImVec2 offset = selectedFrame.offset;

        if (ImGui::SliderFloat2("Offset", (float*)&offset, -100.f, 100.f, "%.1f"))
        {
            // 오프셋이 조정되면 프레임의 위치를 실시간으로 업데이트
            ImVec2 deltaOffset = ImVec2(offset.x - selectedFrame.offset.x, offset.y - selectedFrame.offset.y);
            selectedFrame.offset = offset;
            selectedFrame.topLeft = ImVec2(selectedFrame.topLeft.x + deltaOffset.x, selectedFrame.topLeft.y + deltaOffset.y);
            selectedFrame.bottomRight = ImVec2(selectedFrame.bottomRight.x + deltaOffset.x, selectedFrame.bottomRight.y + deltaOffset.y);
            selectedFrame.UI_topLeft = ImVec2(selectedFrame.UI_topLeft.x + deltaOffset.x, selectedFrame.UI_topLeft.y + deltaOffset.y);
            selectedFrame.UI_bottomRight = ImVec2(selectedFrame.UI_bottomRight.x + deltaOffset.x, selectedFrame.UI_bottomRight.y + deltaOffset.y);
        }

        ImGui::End();
    }
}

void SpriteView::DeleteFrame()
{
    if (!m_Frames.empty())
    {
        m_Frames.pop_back(); // 마지막에 추가된 프레임 제거
    }
}