#include "pch.h"
#include "EditorUI.h"

EditorUI::EditorUI(const string& name, const  string& id)
	: m_Name(name)
	, m_ID(id)
	, m_ParentUI(nullptr)
	, m_Active(true)
	, m_Seperate(false)
	, m_Modal(false)
	, m_IsComponent(false)
	, m_Menu(false)
	, m_Move(true)
{

}

EditorUI::~EditorUI()
{
	// 자식 UI delete
	Safe_Del_Vector(m_vecChildUI);
}

bool EditorUI::SetActive(bool active)
{
	if (m_Active == active)
		return false;

	m_Active = active;

	if (m_Active)
		Activate();
	else
		Deactivate();

	return true;
}

void EditorUI::SetFocus()
{
	string fullname = m_Name + m_ID;

	ImGui::SetWindowFocus(fullname.c_str());
}

void EditorUI::Tick()
{
	// 해당 UI 가 비활성화 상태면 return
	if (!m_Active)
		return;

	string fullname = m_Name + m_ID; // Inspector##Inspector

	// 개별 Flag 설정 (비트 연산)
	UINT flag = 0;

	if (m_Menu)
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;

	if (!m_Move)
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;

	// 자신이 "독립적인 UI" 혹은 "부모 UI" 라면 (최상위 UI 객체)
	if (IsRootUI())
	{
		//if (m_Seperate)
		//{
		//	// 구분선 생성
		//	ImGui::Separator();
		//}

		// 새로 생긴 x 버튼을 누르면 *bool 을 false 로 변경해줌.
		bool bActive = m_Active;

		// Modal Less
		if (!m_Modal)
		{
			ImGui::Begin(fullname.c_str(), &bActive, flag);
			SetActive(bActive);

			Render_Tick();

			// 자식 UI 의 Tick 을 돌려준다.
			for (size_t i = 0; i < m_vecChildUI.size(); i++)
			{
				m_vecChildUI[i]->Tick();
			}

			ImGui::End();
		}
		// Modal Use
		else
		{
			ImGui::OpenPopup(fullname.c_str());
			if (ImGui::BeginPopupModal(fullname.c_str(), &bActive, flag))
			{
				SetActive(bActive);

				Render_Tick();

				// 자식 UI 의 Tick 을 돌려준다.
				for (size_t i = 0; i < m_vecChildUI.size(); i++)
				{
					m_vecChildUI[i]->Tick();
				}

				ImGui::EndPopup();
			}
			else
			{
				SetActive(bActive);
			}
		}
	}
	// 자신이 부모가 있는 "자식 UI" 라면
	else
	{
		if (m_Seperate)
		{
			// 구분선 생성
			//ImGui::Separator();
		}

		if (m_IsComponent)
		{
			if (ImGui::TreeNodeEx(fullname.c_str(), ImGuiTreeNodeFlags_Framed))
			{
				Render_Tick();

				//ImGui::Separator();

				for (size_t i = 0; i < m_vecChildUI.size(); i++)
				{
					m_vecChildUI[i]->Tick();
				}

				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::BeginChild(fullname.c_str(), m_vChildSize);

			Render_Tick();

			for (size_t i = 0; i < m_vecChildUI.size(); i++)
			{
				m_vecChildUI[i]->Tick();
			}

			ImGui::EndChild();
		}		
	}
}