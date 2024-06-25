#include "pch.h"
#include "TextureUI.h"

#include <Engine\\CTexture.h>

TextureUI::TextureUI()
	: AssetUI("TextureUI", "##TextureUI", ASSET_TYPE::TEXTURE)
{

}

TextureUI::~TextureUI()
{

}

void TextureUI::Render_Tick()
{
	Render_Title();

    Ptr<CTexture> pTexture = dynamic_cast<CTexture*>(GetTarget().Get());
    assert(pTexture.Get());

    // Texture 이름
    ImGui::Text("Name"); SAME;
    string strTexKey = ToString(pTexture->GetKey());
    ImGui::InputText("##TextureUIName", (char*)strTexKey.c_str(), strTexKey.capacity(), ImGuiInputTextFlags_ReadOnly);

    // Image
    ImGui::SeparatorText("Texture");
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);

    ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImGui::Image((ImTextureID)pTexture->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, ImGui::GetStyleColorVec4(ImGuiCol_Text), border_col);
    //                -> ImTextureID = Texture Shader Resource View (Tex_SRV)

    // 해상도
    ImGui::SeparatorText("Resolution");

    int Width = pTexture->GetWidth();
    int Height = pTexture->GetHeight();

    ImGui::Text("Width"); SAME;
    ImGui::InputInt("##TextureWidth", &Width, 0, 100, ImGuiInputTextFlags_ReadOnly);
    ImGui::Text("Height"); SAME;
    ImGui::InputInt("##TextureHeight", &Height, 0, 100, ImGuiInputTextFlags_ReadOnly);
}