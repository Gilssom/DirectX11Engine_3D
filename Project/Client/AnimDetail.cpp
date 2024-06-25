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
        // �������� �����Ǹ� �������� ��ġ�� �ǽð����� ������Ʈ
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

    // x �� ����
    if (ImGui::SliderFloat("X", &frameSize.x, 0.0f, 500.f))
    {
        // �巡�׷� ������ �������� ��ġ�� �����ϰ� ũ�⸦ ����
        m_SelectedFrame->bottomRight.x = m_SelectedFrame->topLeft.x + frameSize.x;

        // ũ�Ⱑ ������ ��ŭ ������ ���ؼ� UI �簢���� �ش� ������ŭ �����ϱ�
        float Ratio = frameSize.x / initSize.x;
        m_SelectedFrame->UI_bottomRight.x *= Ratio;
    }

    // y �� ����
    if (ImGui::SliderFloat("Y", &frameSize.y, 0.0f, 500.f))
    {
        // �巡�׷� ������ �������� ��ġ�� �����ϰ� ũ�⸦ ����
        m_SelectedFrame->bottomRight.y = m_SelectedFrame->topLeft.y + frameSize.y;

        // ũ�Ⱑ ������ ��ŭ ������ ���ؼ� UI �簢���� �ش� ������ŭ �����ϱ�
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