#include "pch.h"
#include "Anim3DDetail.h"

#include <Engine\CAnimator3D.h>

Anim3DDetail::Anim3DDetail()
	: Anim3DSubUI("Anim 3D Detail", "##Anim 3D Detail")
	, m_Animator3D(nullptr)
    , m_ZoomLevel(1.f)
    , m_ScrollOffset(0.f)
{

}

Anim3DDetail::~Anim3DDetail()
{
	if(m_Animator3D != nullptr)
		m_Animator3D = nullptr;
}

void Anim3DDetail::DrawTimeLineView(int& curFrame, int maxFrame, float& zoomLevel, float& scrollOffset)
{
    // Ÿ�Ӷ����� ũ��� ��ġ�� ����
    ImVec2 timelinePos = ImGui::GetCursorScreenPos();
    ImVec2 timelineSize = ImVec2(ImGui::GetContentRegionAvail().x, 50); // Ÿ�Ӷ��� ����

    // Ÿ�Ӷ��� ��� �׸���
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(timelinePos, ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y), IM_COL32(60, 60, 60, 255));

    // ���콺 �� �Է��� ������ zoomLevel ������Ʈ
    if (ImGui::IsWindowHovered())
    {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f)
        {
            zoomLevel += wheel * 0.1f;  // ���� ������ ������ zoomLevel�� ����
            zoomLevel = Custom_Clamp(zoomLevel, 0.1f, 5.0f);  // �ּ�/�ִ� �� ���� ����
        }
    }

    // Ȯ��/��ҿ� ���� ������ ���� ���
    float frameStep = (timelineSize.x / maxFrame) * zoomLevel; // zoomLevel�� ���� Ȯ��/���

    // �����̴��� Ÿ�Ӷ��� �¿� ��ũ�� ���� (scrollOffset�� ���콺 �ٰ� �����̴��� ����)
    float maxScrollOffset = (maxFrame * frameStep) - timelineSize.x;
    scrollOffset = Custom_Clamp(scrollOffset, 0.0f, maxScrollOffset);  // ��ũ�� ������ ����

    // �� �������� ���� �׸���
    for (int i = 0; i <= maxFrame; i++)
    {
        float xPos = timelinePos.x + i * frameStep - scrollOffset;  // ��ũ�� ������ ����

        // Ÿ�Ӷ��� ���� ���� ���� ���� �׸���
        if (xPos >= timelinePos.x && xPos <= timelinePos.x + timelineSize.x)
        {
            // 10�����Ӹ��� ū ���ݰ� ��ȣ ǥ��
            if (i % 10 == 0)
            {
                drawList->AddLine(ImVec2(xPos, timelinePos.y), ImVec2(xPos, timelinePos.y + timelineSize.y), IM_COL32(255, 255, 255, 255), 2.0f);

                // ������ ��ȣ �ؽ�Ʈ ǥ��
                char frameLabel[8];
                sprintf_s(frameLabel, "%d", i);
                drawList->AddText(ImVec2(xPos - 10, timelinePos.y + timelineSize.y + 5), IM_COL32(255, 255, 255, 255), frameLabel);
            }
            else
            {
                // ���� ���� ǥ�� (1������ ����)
                drawList->AddLine(ImVec2(xPos, timelinePos.y), ImVec2(xPos, timelinePos.y + timelineSize.y * 0.5f), IM_COL32(255, 255, 255, 150), 1.0f);
            }
        }
    }

    // ���� �������� ǥ���ϴ� �ڵ鷯 (�����̴�ó�� ���̰�)
    float handleX = timelinePos.x + (curFrame / (float)maxFrame) * timelineSize.x * zoomLevel - scrollOffset;
    drawList->AddRectFilled(ImVec2(handleX - 5, timelinePos.y), ImVec2(handleX + 5, timelinePos.y + timelineSize.y), IM_COL32(255, 0, 0, 255));

    // �ڵ鷯 �巡�� ��� �߰�
    static bool isDraggingHandle = false;
    if (ImGui::IsMouseHoveringRect(ImVec2(handleX - 5, timelinePos.y), ImVec2(handleX + 5, timelinePos.y + timelineSize.y)))
    {
        if (ImGui::IsMouseClicked(0))  // ���� Ŭ������ �巡�� ����
        {
            isDraggingHandle = true;
        }
    }
    if (isDraggingHandle)
    {
        if (ImGui::IsMouseDragging(0))  // �巡�� ���� ��
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            float relativePos = (mousePos.x - timelinePos.x + scrollOffset) / (timelineSize.x * zoomLevel);
            curFrame = (int)(relativePos * maxFrame);
            curFrame = Custom_Clamp(curFrame, 0, maxFrame);  // ������ ������ ����� �ʵ��� ����
        }
        if (ImGui::IsMouseReleased(0))  // �巡�� ����
        {
            isDraggingHandle = false;
        }
    }

    // Ÿ�Ӷ��� Ŭ�����ε� ������ ���� ����
    if (ImGui::IsMouseHoveringRect(timelinePos, ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y)) && !isDraggingHandle)
    {
        if (ImGui::IsMouseClicked(0))  // ���� Ŭ�� ��
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            float relativePos = (mousePos.x - timelinePos.x + scrollOffset) / (timelineSize.x * zoomLevel);
            curFrame = (int)(relativePos * maxFrame);
            curFrame = Custom_Clamp(curFrame, 0, maxFrame);  // ������ ������ ����� �ʵ��� ����
        }
    }

    // �����̴��� Ÿ�Ӷ��� �Ʒ��� ǥ��
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 100);  // Ÿ�Ӷ��� �ؿ� �����̴��� ��ġ�ϵ��� ����
    if (maxFrame * frameStep > timelineSize.x)  // Ÿ�Ӷ����� â���� Ŭ ���� �����̴� Ȱ��ȭ
    {
        ImGui::SliderFloat("Scroll", &scrollOffset, 0.0f, maxScrollOffset, "Offset: %.1f");
    }
}

void Anim3DDetail::Render_Tick()
{
	ImGui::SeparatorText("Animation TimeLine");

	m_Animator3D = GetOwner()->GetAnimator3D();

	if (m_Animator3D == nullptr)
		return;

	m_Animator3D->SetEditorMode(true);

	const vector<tMTAnimClip>* pAnimClip = m_Animator3D->GetVecAnimClip();

	int animSize = pAnimClip->size();
	int selectAnimIndex = 0;

	int minFrame = 0;
	int maxFrame = pAnimClip->at(selectAnimIndex).iFrameLength;

	int curFrame = m_Animator3D->GetCurFrameIdx();

    DrawTimeLineView(curFrame, maxFrame, m_ZoomLevel, m_ScrollOffset);

    m_Animator3D->SetEditorFrame(curFrame);

    // zoom Level Setting
    ImGui::SeparatorText("Animation TimeLine Setting");
    ImGui::SliderFloat("Zoom", &m_ZoomLevel, 1.0f, 10.0f);
    ImGui::InputInt("Cur Index", &curFrame);

	//if (ImGui::SliderInt("Anim Frame", &curFrame, 0, maxFrame))
	//{
	//	m_Animator3D->SetEditorFrame(curFrame);
	//}
}

void Anim3DDetail::Activate()
{

}

void Anim3DDetail::Deactivate()
{
	m_Animator3D = nullptr;

	if(GetOwner() != nullptr)
		m_Animator3D = GetOwner()->GetAnimator3D();

	if(m_Animator3D != nullptr)
		m_Animator3D->SetEditorMode(false);

	m_Animator3D = nullptr;
}