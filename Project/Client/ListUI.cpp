#include "pch.h"
#include "ListUI.h"

ListUI::ListUI()
	: EditorUI("List", "##List")
    , m_SelectedIdx(-1)
{

}

ListUI::~ListUI()
{

}

void ListUI::Render_Tick()
{
    // 자신이 속해있는 Window 창의 크기를 알려준다. (List UI 의 영역크기)
    ImVec2 vWinSize = ImGui::GetWindowContentRegionMax();
    vWinSize.y -= 27;
    vWinSize.x -= 8;

    if (ImGui::BeginListBox("##ListBox", vWinSize))
    {
        for (size_t i = 0; i < m_vecList.size(); i++)
        {
            const bool bSelected = (m_SelectedIdx == i);

            if (ImGui::Selectable(m_vecList[i].c_str(), bSelected))
            {
                m_SelectedIdx = i;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                // List UI 는 여러 곳에서 돌려 쓴다.
                // 여러 종류의 기능을 대응하기 위한 함수를 설계해야한다. (CallBack || Delegate - 함수 포인터 Type)
                if (m_CallBack)
                    m_CallBack();

                if(m_Inst && m_Delegate)
                    (m_Inst->*m_Delegate)((DWORD_PTR)&m_vecList[m_SelectedIdx]);

                SetActive(false);
            }
        }

        ImGui::EndListBox();
    }
}

void ListUI::Deactivate()
{
    m_vecList.clear();
    m_SelectedIdx = -1;

    m_CallBack = nullptr;

    m_Inst = nullptr;
    m_Delegate = nullptr;

    SetModal(false);
}
