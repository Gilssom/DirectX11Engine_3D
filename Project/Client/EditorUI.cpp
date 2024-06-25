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
	// �ڽ� UI delete
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
	// �ش� UI �� ��Ȱ��ȭ ���¸� return
	if (!m_Active)
		return;

	string fullname = m_Name + m_ID; // Inspector##Inspector

	// ���� Flag ���� (��Ʈ ����)
	UINT flag = 0;

	if (m_Menu)
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;

	if (!m_Move)
		flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;

	// �ڽ��� "�������� UI" Ȥ�� "�θ� UI" ��� (�ֻ��� UI ��ü)
	if (IsRootUI())
	{
		//if (m_Seperate)
		//{
		//	// ���м� ����
		//	ImGui::Separator();
		//}

		// ���� ���� x ��ư�� ������ *bool �� false �� ��������.
		bool bActive = m_Active;

		// Modal Less
		if (!m_Modal)
		{
			ImGui::Begin(fullname.c_str(), &bActive, flag);
			SetActive(bActive);

			Render_Tick();

			// �ڽ� UI �� Tick �� �����ش�.
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

				// �ڽ� UI �� Tick �� �����ش�.
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
	// �ڽ��� �θ� �ִ� "�ڽ� UI" ���
	else
	{
		if (m_Seperate)
		{
			// ���м� ����
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