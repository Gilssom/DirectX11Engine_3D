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
    // 타임라인의 크기와 위치를 설정
    ImVec2 timelinePos = ImGui::GetCursorScreenPos();
    ImVec2 timelineSize = ImVec2(ImGui::GetContentRegionAvail().x, 50); // 타임라인 높이

    // 타임라인 배경 그리기
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(timelinePos, ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y), IM_COL32(60, 60, 60, 255));

    // 마우스 휠 입력을 감지해 zoomLevel 업데이트
    if (ImGui::IsWindowHovered())
    {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f)
        {
            zoomLevel += wheel * 0.1f;  // 휠을 움직일 때마다 zoomLevel을 조절
            zoomLevel = Custom_Clamp(zoomLevel, 0.1f, 5.0f);  // 최소/최대 줌 레벨 제한
        }
    }

    // 확대/축소에 따른 프레임 간격 계산
    float frameStep = (timelineSize.x / maxFrame) * zoomLevel; // zoomLevel에 따른 확대/축소

    // 슬라이더로 타임라인 좌우 스크롤 조정 (scrollOffset은 마우스 휠과 슬라이더로 조정)
    float maxScrollOffset = (maxFrame * frameStep) - timelineSize.x;
    scrollOffset = Custom_Clamp(scrollOffset, 0.0f, maxScrollOffset);  // 스크롤 오프셋 제한

    // 각 프레임의 눈금 그리기
    for (int i = 0; i <= maxFrame; i++)
    {
        float xPos = timelinePos.x + i * frameStep - scrollOffset;  // 스크롤 오프셋 적용

        // 타임라인 범위 내에 있을 때만 그리기
        if (xPos >= timelinePos.x && xPos <= timelinePos.x + timelineSize.x)
        {
            // 10프레임마다 큰 눈금과 번호 표시
            if (i % 10 == 0)
            {
                drawList->AddLine(ImVec2(xPos, timelinePos.y), ImVec2(xPos, timelinePos.y + timelineSize.y), IM_COL32(255, 255, 255, 255), 2.0f);

                // 프레임 번호 텍스트 표시
                char frameLabel[8];
                sprintf_s(frameLabel, "%d", i);
                drawList->AddText(ImVec2(xPos - 10, timelinePos.y + timelineSize.y + 5), IM_COL32(255, 255, 255, 255), frameLabel);
            }
            else
            {
                // 작은 눈금 표시 (1프레임 단위)
                drawList->AddLine(ImVec2(xPos, timelinePos.y), ImVec2(xPos, timelinePos.y + timelineSize.y * 0.5f), IM_COL32(255, 255, 255, 150), 1.0f);
            }
        }
    }

    // 현재 프레임을 표시하는 핸들러 (슬라이더처럼 보이게)
    float handleX = timelinePos.x + (curFrame / (float)maxFrame) * timelineSize.x * zoomLevel - scrollOffset;
    drawList->AddRectFilled(ImVec2(handleX - 5, timelinePos.y), ImVec2(handleX + 5, timelinePos.y + timelineSize.y), IM_COL32(255, 0, 0, 255));

    // 핸들러 드래그 기능 추가
    static bool isDraggingHandle = false;
    if (ImGui::IsMouseHoveringRect(ImVec2(handleX - 5, timelinePos.y), ImVec2(handleX + 5, timelinePos.y + timelineSize.y)))
    {
        if (ImGui::IsMouseClicked(0))  // 왼쪽 클릭으로 드래그 시작
        {
            isDraggingHandle = true;
        }
    }
    if (isDraggingHandle)
    {
        if (ImGui::IsMouseDragging(0))  // 드래그 중일 때
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            float relativePos = (mousePos.x - timelinePos.x + scrollOffset) / (timelineSize.x * zoomLevel);
            curFrame = (int)(relativePos * maxFrame);
            curFrame = Custom_Clamp(curFrame, 0, maxFrame);  // 프레임 범위를 벗어나지 않도록 제한
        }
        if (ImGui::IsMouseReleased(0))  // 드래그 종료
        {
            isDraggingHandle = false;
        }
    }

    // 타임라인 클릭으로도 프레임 변경 가능
    if (ImGui::IsMouseHoveringRect(timelinePos, ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y)) && !isDraggingHandle)
    {
        if (ImGui::IsMouseClicked(0))  // 왼쪽 클릭 시
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            float relativePos = (mousePos.x - timelinePos.x + scrollOffset) / (timelineSize.x * zoomLevel);
            curFrame = (int)(relativePos * maxFrame);
            curFrame = Custom_Clamp(curFrame, 0, maxFrame);  // 프레임 범위를 벗어나지 않도록 제한
        }
    }

    // 슬라이더를 타임라인 아래에 표시
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 100);  // 타임라인 밑에 슬라이더가 위치하도록 조정
    if (maxFrame * frameStep > timelineSize.x)  // 타임라인이 창보다 클 때만 슬라이더 활성화
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