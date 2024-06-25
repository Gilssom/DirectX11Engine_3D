#include "pch.h"
#include "AnimDetail.h"

AnimDetail::AnimDetail()
    : AnimSubUI("Anim Detail", "##Anim Detail")
    , m_SpriteView(nullptr)
    , m_SelectedFrame(nullptr)
{

}

AnimDetail::~AnimDetail()
{

}

void AnimDetail::Render_Tick()
{
    ImGui::SeparatorText("Adjust Offset");

    if (m_SpriteView == nullptr)
        return;

    if (m_SelectedFrame == nullptr)
        return;

    ImVec2 offset = m_SelectedFrame->offset;

    if (ImGui::SliderFloat2("Offset", (float*)&offset, -100.f, 100.f, "%.05f"))
    {
        // 오프셋이 조정되면 프레임의 위치를 실시간으로 업데이트
        ImVec2 deltaOffset = ImVec2(offset.x - m_SelectedFrame->offset.x, offset.y - m_SelectedFrame->offset.y);
        m_SelectedFrame->offset = offset;
        m_SelectedFrame->topLeft = ImVec2(m_SelectedFrame->topLeft.x + deltaOffset.x, m_SelectedFrame->topLeft.y + deltaOffset.y);
        m_SelectedFrame->bottomRight = ImVec2(m_SelectedFrame->bottomRight.x + deltaOffset.x, m_SelectedFrame->bottomRight.y + deltaOffset.y);
        m_SelectedFrame->UI_topLeft = ImVec2(m_SelectedFrame->UI_topLeft.x + deltaOffset.x, m_SelectedFrame->UI_topLeft.y + deltaOffset.y);
        m_SelectedFrame->UI_bottomRight = ImVec2(m_SelectedFrame->UI_bottomRight.x + deltaOffset.x, m_SelectedFrame->UI_bottomRight.y + deltaOffset.y);
    }

    ImVec2 frameSize = ImVec2(m_SelectedFrame->bottomRight.x - m_SelectedFrame->topLeft.x,
                              m_SelectedFrame->bottomRight.y - m_SelectedFrame->topLeft.y);

    ImVec2 initSize = frameSize;

    // x 값 조절
    if (ImGui::SliderFloat("X", &frameSize.x, 0.0f, 500.f))
    {
        // 드래그로 설정된 프레임의 위치를 유지하고 크기를 조절
        m_SelectedFrame->bottomRight.x = m_SelectedFrame->topLeft.x + frameSize.x;

        // 크기가 조절된 만큼 비율을 구해서 UI 사각형도 해당 비율만큼 조절하기
        float Ratio = frameSize.x / initSize.x;
        m_SelectedFrame->UI_bottomRight.x *= Ratio;
    }

    // y 값 조절
    if (ImGui::SliderFloat("Y", &frameSize.y, 0.0f, 500.f))
    {
        // 드래그로 설정된 프레임의 위치를 유지하고 크기를 조절
        m_SelectedFrame->bottomRight.y = m_SelectedFrame->topLeft.y + frameSize.y;

        // 크기가 조절된 만큼 비율을 구해서 UI 사각형도 해당 비율만큼 조절하기
        float Ratio = frameSize.y / initSize.y;
        m_SelectedFrame->UI_bottomRight.y *= Ratio;
    }
}

void AnimDetail::Activate()
{
}

void AnimDetail::Deactivate()
{
    m_SpriteView = nullptr;
    m_SelectedFrame = nullptr;
}