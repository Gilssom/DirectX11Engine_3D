#include "pch.h"
#include "Anim3DPreView.h"

Anim3DPreView::Anim3DPreView()
	: Anim3DSubUI("Anim 3D PreView", "##Anim 3D PreView")
{

}

Anim3DPreView::~Anim3DPreView()
{

}

void Anim3DPreView::Render_Tick()
{
	ImGui::SeparatorText("Animation Preview");

    // SkyBox Texture
    Ptr<CTexture> pTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"ColorTargetTex");
    ImTextureID TexID = 0;
    string strTexKey;

    if (pTex != nullptr)
    {
        TexID = pTex->GetSRV().Get();
        strTexKey = ToString(pTex->GetKey());
    }
    else
    {
        strTexKey = "None";
    }

    // Texture ¿Ã∏ß
    ImGui::Text("Anim Preview Texture"); SAME;
    ImGui::InputText("##TextureUIName", (char*)strTexKey.c_str(), strTexKey.capacity(), ImGuiInputTextFlags_ReadOnly);

    // Image
    ImGui::SeparatorText("Texture");
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);

    ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImGui::Image(TexID, ImVec2(300, 300), uv_min, uv_max, ImGui::GetStyleColorVec4(ImGuiCol_Text), border_col);
}

void Anim3DPreView::Activate()
{

}

void Anim3DPreView::Deactivate()
{

}