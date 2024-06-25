#include "pch.h"
#include "Animator2DUI.h"

#include "CImGuiManager.h"
#include "AnimationEditor.h"
#include "ListUI.h"

#include <Engine\\CAnimator2D.h>
#include <Engine\\CAnim2D.h>

Animator2DUI::Animator2DUI()
	: ComponentUI("Animator2D", "##Animator2DUI", COMPONENT_TYPE::ANIMATOR2D)
{
	SIZE;
}

Animator2DUI::~Animator2DUI()
{
}

void Animator2DUI::Render_Tick()
{
	Render_Title();

	ImGui::SeparatorText("Animator");
	ImGui::Text("Animation Editor");
	if (ImGui::Button("Open Editor##OpenTileMapEditor"))
	{
		AnimationEditor* pEditor = CImGuiManager::GetInst()->FindEditorUI<AnimationEditor>("AnimationEditor");
		pEditor->SetActive(true);
	}

	CAnimator2D* pAnimator = GetTarget()->Animator2D();
	CAnim2D* pAnim = pAnimator->GetCurAnim();

	string strAnimKey;
	if (pAnim != nullptr)
	{
		strAnimKey = string(pAnim->GetName().begin(), pAnim->GetName().end());
	}

	ImGui::Text("");

	ImGui::SeparatorText("Animation Info");
	ImGui::Text("Animation  ");
	ImGui::PushItemWidth(200.f);
	ImGui::Text("Play Anim"); SAME;
	ImGui::InputText("##AnimName", (char*)strAnimKey.c_str(), strAnimKey.capacity(), ImGuiInputTextFlags_ReadOnly);
	ImGui::PopItemWidth();

	bool isRepeat = pAnimator->GetRepeat();
	ImGui::Text("Is Repeat"); SAME;
	if (ImGui::Checkbox("##Is Repeat", &isRepeat))
	{
		pAnimator->SetRepeat(isRepeat);
	}

	// Cur Anim Change
	ImGui::Text("Select Anim"); SAME;
	if (ImGui::Button("Anim Select##AnimationButton"))
	{
		// 파일 선택창 열고,
		wchar_t Buff[255] = {};

		OPENFILENAME ofn = {};

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = Buff;
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
			wstring RelativePath = CPathManager::GetInst()->GetRelativePath(Buff);

			pAnimator->LoadAnimation(RelativePath);
			pAnim = pAnimator->FindAnimFile(RelativePath);

			wstring animName = pAnim->GetName();
			pAnimator->Play(animName, pAnimator->GetRepeat());
		}
	}
}