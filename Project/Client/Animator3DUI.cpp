#include "pch.h"
#include "Animator3DUI.h"

#include "CImGuiManager.h"
#include "Animation3DEditor.h"
#include "ListUI.h"

Animator3DUI::Animator3DUI()
	: ComponentUI("Animator3D", "##Animator3DUI", COMPONENT_TYPE::ANIMATOR3D)
{
	SIZE;
}

Animator3DUI::~Animator3DUI()
{
}

void Animator3DUI::Render_Tick()
{
	Render_Title();

	ImGui::SeparatorText("Animator");
	ImGui::Text("Animation 3D Editor");
	if (ImGui::Button("Open Editor##OpenTileMapEditor"))
	{
		Animation3DEditor* pEditor = CImGuiManager::GetInst()->FindEditorUI<Animation3DEditor>("3DAnimationEditor");
		pEditor->SetAnimator3D(GetTarget()->Animator3D());
		pEditor->SetActive(true);
	}
}