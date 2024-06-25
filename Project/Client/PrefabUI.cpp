#include "pch.h"
#include "PrefabUI.h"

#include <Engine\\CPrefab.h>

PrefabUI::PrefabUI()
	: AssetUI("PrefabUI", "##PrefabUI", ASSET_TYPE::PREFAB)
{

}

PrefabUI::~PrefabUI()
{

}

void PrefabUI::Render_Tick()
{
	Render_Title();

	Ptr<CPrefab> pPrefab = dynamic_cast<CPrefab*>(GetTarget().Get());
	assert(pPrefab.Get());

	// Texture ÀÌ¸§
	ImGui::Text("Name"); SAME;
	string strPrefabKey = ToString(pPrefab->GetKey());
	ImGui::InputText("##PrefabUIName", (char*)strPrefabKey.c_str(), strPrefabKey.capacity(), ImGuiInputTextFlags_ReadOnly);
}