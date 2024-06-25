#include "pch.h"
#include "CameraUI.h"

#include <Engine\\CRenderManager.h>
#include <Engine\\CCamera.h>
#include <Engine\\CLevelManager.h>
#include <Engine\\CLevel.h>
#include <Engine\\CLayer.h>

CameraUI::CameraUI()
	: ComponentUI("Camera", "##CameraUI", COMPONENT_TYPE::CAMERA)
{
    SIZE_SET(250);
}

CameraUI::~CameraUI()
{

}

void CameraUI::Render_Tick()
{
	Render_Title();

    CCamera* pCam = GetTarget()->Camera();

	// ���� ��� Projection Type
    PROJ_TYPE ProjType = pCam->GetProjType();
    bool IsPerspective = ProjType == PROJ_TYPE::PERSPECTIVE;
    bool IsOrthographic = ProjType == PROJ_TYPE::ORTHOGRAPHIC;

    const char* items[2] = {};

    items[0] = "Orthographic";
    items[1] = "Perspective";

    ImGui::Text("Projection Type"); SAME;
    if (ImGui::BeginCombo("##CameraProjType", items[(UINT)ProjType], 0))
    {
        for (int i = 0; i < 2; i++)
        {
            const bool is_selected = ((UINT)ProjType == i);
            if (ImGui::Selectable(items[i], is_selected))
            {
                pCam->SetProjType((PROJ_TYPE)i);
            }
        }

        ImGui::EndCombo();
    }


    // Camera Priority (�켱����)
    // Render Manager �� ���� ī�޶� ��� �ִ� Capacity �� �˾Ƴ���.
    vector<CCamera*>& vecCam = CRenderManager::GetInst()->GetRegisteredCamera();
    
    ImGui::Text("Priority"); SAME;
    int iPriority = pCam->GetCameraPriority();
    if (ImGui::InputInt("##CameraPriority", &iPriority))
    {
        // ī�޶� �켱���� ���� �����ߴٸ�
        int CurPriority = pCam->GetCameraPriority();

        // �����Ϸ��� ī�޶� �켱���� ���̶� ������ ī�޶� �ش� Level �ȿ� �ִ��� Ȯ�� �� �����Ѵ�.
        CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
        vector<CGameObject*> vecCamObject;
        pCurLevel->FindObjectByComponent(COMPONENT_TYPE::CAMERA, vecCamObject);

        // ������ �켱������ ������ �ִ� �ٸ� ī�޶� �ִٸ�
        // �����Ϸ��� ī�޶��� �켱���� ���� ���� �ְ�,
        for (size_t i = 0; i < vecCamObject.size(); i++)
        {
            if (vecCamObject[i]->Camera()->GetCameraPriority() == iPriority)
            {
                vecCamObject[i]->Camera()->SetCameraPriority(CurPriority);
                break;
            }
        }

        // ���� ī�޶�� ���ο� �켱������ ����
        pCam->SetCameraPriority(iPriority);
    }


	// Camera FOV (�þ߰���_���� ����������)
    float FOV = pCam->GetFOV();
    FOV = (FOV * 180.f) / XM_PI;
    ImGui::Text("FOV"); SAME;

    ImGui::BeginDisabled(IsOrthographic);

    if (ImGui::DragFloat("##CameraFOV", &FOV, 0.1f, 0.f, 0.f, "%.3f"))
    {
        FOV = (FOV * XM_PI) / 180.f;
        pCam->SetFOV(FOV);
    }

    ImGui::EndDisabled();


	// Camera Far
    float Far = pCam->GetFar();
    ImGui::Text("Far"); SAME;

    if (ImGui::DragFloat("##CameraFar", &Far, 1.f))
    {
        if (Far < 2.f)
            Far = 2.f;

        pCam->SetFar(Far);
    }

	// Camera Width
    ImGui::Text("Width"); SAME;
    float Width = pCam->GetWidth();
    if (ImGui::DragFloat("##CameraWidth", &Width, 1.f))
    {
        if (Width < 1.f)
            Width = 1.f;

        pCam->SetWidth(Width);
    }
    
	// Camera AspectRatio
    ImGui::Text("AspectRatio"); SAME;
    float AspectRatio = pCam->GetAspectRatio();
    if (ImGui::DragFloat("##CameraAspectRatio", &AspectRatio, 0.1f))
    {
        if (AspectRatio < 0.f)
            AspectRatio = 0.f;

        pCam->SetAspectRatio(AspectRatio);
    }

	// Camera Scale(Orthographic - ���� ����_���� ����������)
    ImGui::Text("Projection Scale"); SAME;
    float Scale = pCam->GetScale();
    
    ImGui::BeginDisabled(IsPerspective);

    if (ImGui::DragFloat("##CameraScale", &Scale, 0.01f))
    {
        if (Scale < 0.1f)
            Scale = 0.1f;

        pCam->SetScale(Scale);
    }

    ImGui::EndDisabled();

	// Camera Layer Check
    ImGui::Text("Layer Check"); SAME;

    UINT LayerCheck = pCam->GetLayerCheck();
    CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

    if (pCurLevel == nullptr)
        return;

    bool CurLayerCheck[MAX_LAYER] = {};

    for (size_t i = 0; i < MAX_LAYER; i++)
    {
        if (LayerCheck & (1 << i))
            CurLayerCheck[i] = true;
        else
            CurLayerCheck[i] = false;
    }

    vector<string> vecLayerName;

    for (size_t i = 0; i < MAX_LAYER; i++)
    {
        string curLayerName = ToString(pCurLevel->GetLayer(i)->GetName());
        string strLayerName;
        char buffer[256] = {};
        sprintf_s(buffer, 256, "Layer Index %d : ", i);
        strLayerName = buffer;

        if (!curLayerName.empty())
        {
            strLayerName += curLayerName;
            vecLayerName.push_back(strLayerName);
        }
        else
        {
            strLayerName += "null";
            vecLayerName.push_back(strLayerName);
        }
    }

    if (ImGui::Button("Layer Check"))
        ImGui::OpenPopup("Layer Check Popup");

    if (ImGui::BeginPopup("Layer Check Popup"))
    {
        for (size_t i = 0; i < vecLayerName.size(); i++)
        {
            if (ImGui::MenuItem(vecLayerName[i].c_str(), "", &CurLayerCheck[i]))
            {
                pCam->LayerCheck(i);
            }
        }

        ImGui::Separator();
        ImGui::Text("Select Show Layer List");

        ImGui::EndPopup();
    }

    SAME_SET(250);
    if (ImGui::Button("Layer All Check"))
    {
        if(CurLayerCheck[0] == false)
            pCam->LayerCheckAll(0xffffffff);
        else
            pCam->LayerCheckAll(0);
    }

    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("All Layer Check\n"
            "If even one layer is Off, All set On.\n"
            "If even all layer is On, All set Off.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}