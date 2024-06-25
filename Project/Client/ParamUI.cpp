#include "pch.h"
#include "ParamUI.h"

#include "ImGui\\imgui.h"
#include "CImGuiManager.h"

#include "ListUI.h"
#include <Engine\CGameObject.h>

UINT    ParamUI::g_ParamUI_ID = 0;
int     ParamUI::g_DescWidth = 0;

EditorUI*       ParamUI::g_Inst = nullptr;
UI_DELEGATE1    ParamUI::g_Delegate_1 = nullptr;

int ParamUI::Param_DragInt(const string& strName, int* pInOut, int speed)
{
    // Parmeter Name
    ImGui::Text(strName.c_str()); ImGui::SameLine(g_DescWidth);

    char key[255] = {};
    sprintf_s(key, 255, "##DragInt_%d", g_ParamUI_ID++);

    if (ImGui::DragInt(key, pInOut, speed))
    {
        return true;
    }

    return false;
}

int ParamUI::Param_DragFloat(const string& strName, float* pInOut, float speed)
{
    // Parmeter Name
    ImGui::Text(strName.c_str()); ImGui::SameLine(g_DescWidth);

    char key[255] = {};
    sprintf_s(key, 255, "##DragFloat_%d", g_ParamUI_ID++);

    if (ImGui::DragFloat(key, pInOut, speed))
    {
        return true;
    }

    return false;
}

int ParamUI::Param_DragVec2(const string& strName, Vec2* pInOut, float speed)
{
    // Parmeter Name
    ImGui::Text(strName.c_str()); ImGui::SameLine(g_DescWidth);

    char key[255] = {};
    sprintf_s(key, 255, "##DragVec2_%d", g_ParamUI_ID++);

    float arr[2] = { pInOut->x, pInOut->y };
    if (ImGui::DragFloat2(key, arr, speed))
    {
        pInOut->x = arr[0];
        pInOut->y = arr[1];
        return true;
    }

    return false;
}

int ParamUI::Param_DragVec4(const string& strName, Vec4* pInOut, float speed)
{
    // Parmeter Name
    ImGui::Text(strName.c_str()); ImGui::SameLine(g_DescWidth);

    char key[255] = {};
    sprintf_s(key, 255, "##DragVec4_%d", g_ParamUI_ID++);

    if (ImGui::DragFloat4(key, *pInOut, speed))
    {
        return true;
    }

    return false;
}

int ParamUI::Param_DragMat(const string& strName, Matrix* pInOut, float speed)
{
    return false;
}

int ParamUI::Param_Texture(const string& strName, Ptr<CTexture>& texture)
{
    int btnClicked = false;

    // Parmeter Name
    ImGui::Text(strName.c_str()); ImGui::SameLine(g_DescWidth);

    char key[255] = {};
    sprintf_s(key, 255, "##InputText_%d", g_ParamUI_ID++);

    // Texture Name
    string strTexName;
    if (texture != nullptr)
        strTexName = ToString(texture->GetKey());

    ImGui::SetNextItemWidth(200);
    ImGui::InputText(key, (char*)strTexName.c_str(), strTexName.capacity(), ImGuiInputTextFlags_ReadOnly);

    // Drag Drop
    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content");

        if (nullptr != payload)
        {
            DWORD_PTR dwData = 0;
            memcpy(&dwData, payload->Data, payload->DataSize);

            Ptr<CAsset> pAsset = (CAsset*)dwData;
            if (pAsset->GetAssetType() == ASSET_TYPE::TEXTURE)
            {
                texture = (CTexture*)pAsset.Get();
            }
        }

        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();

    // ListUI 버튼 추가
    sprintf_s(key, 255, "##TextureBtn_%d", g_ParamUI_ID++);
    if (ImGui::Button(key, ImVec2(18, 18)))
    {
        // ListUI 를 활성화한다
        ListUI* pListUI = CImGuiManager::GetInst()->FindEditorUI<ListUI>("List");

        if (pListUI != nullptr && !pListUI->IsActive())
        {
            pListUI->SetModal(true);
            pListUI->SetActive(true);
            pListUI->SetFocus();

            vector<string> vecNames;
            CAssetManager::GetInst()->GetAssetNames(ASSET_TYPE::TEXTURE, vecNames);
            pListUI->AddItem(vecNames);

            if (g_Inst && g_Delegate_1)
                pListUI->RegisterDoubleClickDelegate(g_Inst, g_Delegate_1);

            g_Inst = nullptr;
            g_Delegate_1 = nullptr;
        }

        btnClicked = true;
    }


    // Image 출력
    ImTextureID TexID = (nullptr == texture) ? nullptr : texture->GetSRV().Get();
    ImVec4 BorderColor = (nullptr == texture) ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(1.f, 1.f, 1.f, 0.f);
    
    ImGui::Image( TexID
                , ImVec2(150.f, 150.f)
                , ImVec2(0.0f, 0.0f)
                , ImVec2(1.0f, 1.0f)
                , ImGui::GetStyleColorVec4(ImGuiCol_Text)
                , BorderColor);

    return btnClicked;
}

int ParamUI::Param_GameObject(const string& strName, CGameObject*& object)
{
    // Parmeter Name
    ImGui::Text(strName.c_str()); ImGui::SameLine(g_DescWidth);

    char key[255] = {};
    sprintf_s(key, 255, "##InputGameObject_%d", g_ParamUI_ID++);

    // GameObject Name
    string strObjectName = "null";
    //if (object != nullptr)
    //    strObjectName = ToString(object->GetName());
    //else
    //    strObjectName = "null";

    ImGui::SetNextItemWidth(200);
    ImGui::InputText(key, (char*)strObjectName.c_str(), strObjectName.capacity(), ImGuiInputTextFlags_ReadOnly);

    // Drag Drop
    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Outliner");

        if (nullptr != payload)
        {
            DWORD_PTR dwData = 0;
            memcpy(&dwData, payload->Data, payload->DataSize);

            CGameObject* pObject = (CGameObject*)dwData;
            object = pObject;
        }

        ImGui::EndDragDropTarget();
    }

    return 0;
}
