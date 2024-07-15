#include "pch.h"
#include "DecalUI.h"

#include <Engine\\CDecal.h>

DecalUI::DecalUI()
	: ComponentUI("Decal", "##DecalUI", COMPONENT_TYPE::DECAL)
{

}

DecalUI::~DecalUI()
{

}

void DecalUI::Render_Tick()
{
	Render_Title();

	CDecal* pDecal = GetTarget()->Decal();

    // Decal Texture
    Ptr<CTexture> pTex = pDecal->GetOutputTexture();
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

    // Texture �̸�
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
        // Content �κ��� ����� ��ü���� üũ
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content");

        // ���� ������� �ùٸ� ��ü���
        if (payload != nullptr)
        {
            // payload �� ����� Data �� �츮 ������� �����;� �Ѵ�.
            DWORD_PTR dwData = 0;
            memcpy(&dwData, payload->Data, payload->DataSize);

            // �ش� ��ü�� Type �� �ڽ��� Type �� ��
            Ptr<CAsset> pAsset = (CAsset*)dwData;
            if (pAsset->GetAssetType() == ASSET_TYPE::TEXTURE)
            {
                pDecal->SetOutputTexture((CTexture*)pAsset.Get());
            }
        }

        ImGui::EndDragDropTarget();
    }

    // AsLight
	bool isAsLight = pDecal->GetAsLight();
	ImGui::Text("Is As Light"); SAME;
	if (ImGui::Checkbox("##Is As Light", &isAsLight))
	{
		pDecal->SetAsLight(isAsLight);
	}

    // Light Power
    float power = pDecal->GetDecalPower();
    ImGui::Text("Decal Power"); SAME;

    if (ImGui::DragFloat("##CameraFar", &power, 0.01f, 0.f, 1.f, "%.2f"))
    {
        if (power > 1.f)
            power = 1.f;
        else if (power < 0.f)
            power = 0.f;

        pDecal->SetDecalPower(power);
    }
}