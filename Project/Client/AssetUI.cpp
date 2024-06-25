#include "pch.h"
#include "AssetUI.h"

AssetUI::AssetUI(const string& name, const string& id, ASSET_TYPE type)
	: EditorUI(name, id)
	, m_Type(type)
{

}

AssetUI::~AssetUI()
{

}

void AssetUI::SetTarget(Ptr<CAsset> target)
{
	if (m_TargetAsset == target)
		return;

	m_TargetAsset = target;

	TargetChanged();
}

void AssetUI::Render_Tick()
{

}

void AssetUI::Render_Title()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.f / 4.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3.f / 4.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3.f / 4.0f, 0.7f, 0.7f));

	ImGui::Button(ASSET_TYPE_STRING[(UINT)GetAssetType()]);
	ImGui::PopStyleColor(3);
	ImGui::PopID();
}