#include "pch.h"
#include "Animation3DEditor.h"

#include "CImGuiManager.h"
#include "Anim3DSubUI.h"

#include "Anim3DPreView.h"
#include "Anim3DDetail.h"

Animation3DEditor::Animation3DEditor()
	: EditorUI("3DAnimationEditor", "##3DAnimationEditor")
	, m_Sub{}
	, m_PrevActive{}
{
	//m_Sub[SPRITEVIEW] = CImGuiManager::GetInst()->FindEditorUI<SpriteView>("Sprite View");
	//m_Sub[PREVIEW_3D] = CImGuiManager::GetInst()->FindEditorUI<Anim3DPreView>("Anim 3D PreView");
	//m_Sub[ANIMDETAIL] = CImGuiManager::GetInst()->FindEditorUI<AnimDetail>("Anim Detail");

	m_Sub[PREVIEW_3D] = CImGuiManager::GetInst()->FindEditorUI<Anim3DPreView>("Anim 3D PreView");
	m_Sub[DETAIL_3D] = CImGuiManager::GetInst()->FindEditorUI<Anim3DDetail>("Anim 3D Detail");

	for (int i = 0; i < 3; i++)
	{
		if (m_Sub[i] != nullptr)
		{
			m_Sub[i]->SetActive(true);
			m_Sub[i]->m_Owner = this;
		}
	}
}

Animation3DEditor::~Animation3DEditor()
{

}

void Animation3DEditor::Render_Tick()
{
	//bool IsActive[3] = { m_Sub[0]->IsActive(), m_Sub[1]->IsActive(), m_Sub[2]->IsActive() };

	bool IsActive[3] = { false, m_Sub[1]->IsActive() , m_Sub[2]->IsActive() };

	m_Sub[PREVIEW_3D]->SetActive(IsActive[PREVIEW_3D]);
	m_Sub[DETAIL_3D]->SetActive(IsActive[DETAIL_3D]);

	if (ImGui::BeginMenu("File"))
	{
		Anim3DDetail* pAnimDetail = (Anim3DDetail*)m_Sub[DETAIL_3D];

		if (ImGui::MenuItem("Save File"))
		{
			if (pAnimDetail == nullptr)
				return;

			wchar_t Buff[255] = {};

			OPENFILENAME ofn = {};
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = Buff;        // 결과 출력
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = 255;
			ofn.lpstrFilter = L"Animation\0*.anim\0All\0*.*";
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;

			// 탐색창 초기 위치 지정
			wstring strInitPath = CPathManager::GetInst()->GetContentPath() + wstring(L"Animation\\");
			ofn.lpstrInitialDir = strInitPath.c_str();
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetSaveFileName(&ofn))
			{
				wstring RelativeName = CPathManager::GetInst()->GetRelativeName(Buff, wstring(L"Animation\\"));

				m_Animator3D->SetName(RelativeName);
				m_Animator3D->Save(L"Animation\\");
			}
		}


		// Load 기능 추가
		if (ImGui::MenuItem("Load File"))
		{
			if (pAnimDetail == nullptr)
				return;

			wchar_t Buff[255] = {};

			OPENFILENAME ofn = {};
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = Buff;        // 결과 출력
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = 255;
			ofn.lpstrFilter = L"Animation\0*.anim\0All\0*.*";
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;

			// 탐색창 초기 위치 지정
			wstring strInitPath = CPathManager::GetInst()->GetContentPath() + wstring(L"Animation\\");
			ofn.lpstrInitialDir = strInitPath.c_str();
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn))
			{
				wstring RelativeName = CPathManager::GetInst()->GetRelativeName(Buff, wstring(L"Animation\\"));

				m_Animator3D->SetName(RelativeName);
				m_Animator3D->Load(L"Animation\\");  // 불러오기 함수 호출
			}
		}

		ImGui::EndMenu();
	}
}

void Animation3DEditor::Activate()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_PrevActive[i])
		{
			if (m_Sub[i] != nullptr)
				m_Sub[i]->SetActive(true);
		}
	}
}

void Animation3DEditor::Deactivate()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_Sub[i] != nullptr)
		{
			m_PrevActive[i] = m_Sub[i]->IsActive();
			m_Sub[i]->SetActive(false);
		}
	}
}