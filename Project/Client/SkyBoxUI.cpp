#include "pch.h"
#include "SkyBoxUI.h"

#include <Engine\\CSkyBox.h>

SkyBoxUI::SkyBoxUI()
	: ComponentUI("SkyBox", "##SkyBoxUI", COMPONENT_TYPE::SKYBOX)
{

}

SkyBoxUI::~SkyBoxUI()
{

}

void SkyBoxUI::Render_Tick()
{
	Render_Title();

	CSkyBox* pSkyBox = GetTarget()->SkyBox();

    // SkyBox Type
	SKYBOX_TYPE type = pSkyBox->GetSkyBoxType();

    const char* items[2] = {};

    items[0] = "Sphere";
    items[1] = "Cube";

    ImGui::Text("SkyBox Type"); SAME;
    if (ImGui::BeginCombo("##SkyBoxUI", items[(UINT)type], 0))
    {
        for (int i = 0; i < 2; i++)
        {
            const bool is_selected = ((UINT)type == i);
            if (ImGui::Selectable(items[i], is_selected))
            {
                pSkyBox->SetSkyBoxType((SKYBOX_TYPE)i);
            }
        }

        ImGui::EndCombo();
    }

    // SkyBox Texture
    Ptr<CTexture> pTex = pSkyBox->GetSkyBoxTexture();
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

    // Texture 이름
    ImGui::Text("SkyBox Texture"); SAME;
    ImGui::InputText("##TextureUIName", (char*)strTexKey.c_str(), strTexKey.capacity(), ImGuiInputTextFlags_ReadOnly);

    // Image
    ImGui::SeparatorText("Texture");
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);

    ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImGui::Image(TexID, ImVec2(150, 150), uv_min, uv_max, ImGui::GetStyleColorVec4(ImGuiCol_Text), border_col);

    if (ImGui::BeginDragDropTarget())
    {
        // Content 로부터 날라온 물체인지 체크
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content");

        // 만약 출발지가 올바른 물체라면
        if (payload != nullptr)
        {
            // payload 에 저장된 Data 를 우리 방식으로 꺼내와야 한다.
            DWORD_PTR dwData = 0;
            memcpy(&dwData, payload->Data, payload->DataSize);

            // 해당 물체의 Type 과 자신의 Type 을 비교
            Ptr<CAsset> pAsset = (CAsset*)dwData;
            if (pAsset->GetAssetType() == ASSET_TYPE::TEXTURE)
            {
                pSkyBox->SetSkyBoxTexture((CTexture*)pAsset.Get());
            }
        }

        ImGui::EndDragDropTarget();
    }
}