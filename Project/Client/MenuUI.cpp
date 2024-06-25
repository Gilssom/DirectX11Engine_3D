#include "pch.h"
#include "MenuUI.h"

#include <Engine\\CEngine.h>
#include <Engine\\CLevelManager.h>
#include <Engine\\CLevel.h>
#include <Engine\\CLayer.h>
#include <Engine\\CGameObject.h>
#include <Engine\\components.h>
#include <Engine\\CScript.h>
#include <Engine\\CPathManager.h>

#include <Scripts\\CScriptManager.h>

#include "CLevelSaveLoad.h"

#include "CImGuiManager.h"
#include "Inspector.h"
#include "ContentUI.h"

MenuUI::MenuUI()
	: EditorUI("Menu", "##Menu")
{

}

MenuUI::~MenuUI()
{

}

void MenuUI::Tick()
{
	if (ImGui::BeginMainMenuBar())
	{
		File();

		Level();

		GameObject();

        Asset();

		ImGui::EndMainMenuBar();
	}
}

void MenuUI::Render_Tick() { }

void MenuUI::File()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Exit", "Alt+F4"))
        {
            DestroyWindow(CEngine::GetInst()->GetMainWnd());
        }

        ImGui::EndMenu();
    }
}

void MenuUI::Level()
{
    if (ImGui::BeginMenu("Level"))
    {
        if (ImGui::MenuItem("Save Level"))
        {
            wchar_t Buff[255] = {};

            OPENFILENAME ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = nullptr;
            ofn.lpstrFile = Buff;        // 결과 출력
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = 255;
            ofn.lpstrFilter = L"Level\0*.lv\0All\0*.*";
            ofn.nFilterIndex = 0;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;

            // 탐색창 초기 위치 지정
            wstring strInitPath = CPathManager::GetInst()->GetContentPath() + wstring(L"Level\\");
            ofn.lpstrInitialDir = strInitPath.c_str();
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetSaveFileName(&ofn))
            {
                CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
                if (pCurLevel != nullptr)
                {
                    CLevelSaveLoad::SaveLevel(pCurLevel, Buff);
                }
            }    
        }

        if (ImGui::MenuItem("Load Level"))
        {
            wchar_t Buff[255] = {};

            OPENFILENAME ofn = {};

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = nullptr;
            ofn.lpstrFile = Buff;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = 255;
            ofn.lpstrFilter = L"Level\0*.lv\0All\0*.*";
            ofn.nFilterIndex = 0;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;

            // 탐색창 초기 위치 지정
            wstring strInitPath = CPathManager::GetInst()->GetContentPath() + wstring(L"Level\\");;
            ofn.lpstrInitialDir = strInitPath.c_str();
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(Buff);
                ChangeLevelRegister(pLoadedLevel, LEVEL_STATE::STOP);
            }            
        }

        ImGui::Separator();

        CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
        CSound* pCurBgm = pCurLevel->GetBGM();
        bool IsCurLevel = pCurLevel;
        bool IsPlayState = pCurLevel->GetState() == LEVEL_STATE::PLAY;
        bool IsPauseState = pCurLevel->GetState() == LEVEL_STATE::PAUSE;
        bool IsStopState = pCurLevel->GetState() == LEVEL_STATE::STOP;

        // Play
        ImGui::BeginDisabled(IsPlayState);
        if (ImGui::MenuItem("Play"))
        {
            ChangeLevelState(LEVEL_STATE::PLAY);

            if (IsStopState)
            {
                CLevel* pLevel = CLevelManager::GetInst()->GetCurrentLevel();
                pLevel->SetBGM(pCurBgm);
                wstring LevelPath = CPathManager::GetInst()->GetContentPath();
                LevelPath += L"Level\\temp.lv";
                CLevelSaveLoad::SaveLevel(pLevel, LevelPath);
            }
        }
        ImGui::EndDisabled();

        // Pause
        ImGui::BeginDisabled(!IsPlayState);
        if (ImGui::MenuItem("Pause"))
        {
            ChangeLevelState(LEVEL_STATE::PAUSE);
        }
        ImGui::EndDisabled();

        // Stop
        ImGui::BeginDisabled(IsStopState);
        if (ImGui::MenuItem("Stop"))
        {
            wstring LevelPath = CPathManager::GetInst()->GetContentPath();
            LevelPath += L"Level\\temp.lv";
            CLevel* pNextLevel = CLevelSaveLoad::LoadLevel(LevelPath);
            pNextLevel->SetBGM(pCurBgm);

            ChangeLevelRegister(pNextLevel, LEVEL_STATE::STOP);

            // Inspector Target Object Clear
            Inspector* pInspector = CImGuiManager::GetInst()->FindEditorUI<Inspector>("Inspector");
            pInspector->SetTargetObject(nullptr);
        }
        ImGui::EndDisabled();

        ImGui::EndMenu();
    }
}

void MenuUI::GameObject()
{
    if (ImGui::BeginMenu("GameObject"))
    {
        if (ImGui::MenuItem("Create EmptyObject"))
        {
            CGameObject* pNewObj = new CGameObject;
            pNewObj->AddComponent(new CTransform);
            pNewObj->SetName(L"Empty GameObject");
            SpawnObject(0, pNewObj);

            Inspector* pInspector = CImGuiManager::GetInst()->FindEditorUI<Inspector>("Inspector");
            pInspector->SetTargetObject(pNewObj);
        }

        if (ImGui::MenuItem("Register As Prefab"))
        {
            Inspector* pInspector = CImGuiManager::GetInst()->FindEditorUI<Inspector>("Inspector");
            CGameObject* pTargetObj = pInspector->GetTargetObject();
            wstring filePath = CPathManager::GetInst()->GetContentPath();

            //ImGui::BeginDisabled(!pTargetObj);

            Ptr<CPrefab> prefab = new CPrefab(pTargetObj);
            CAssetManager::GetInst()->AddAsset<CPrefab>(pTargetObj->GetName(), prefab);
            prefab->Save(filePath + L"Prefab\\" + pTargetObj->GetName() + L".pref");

            //ImGui::EndDisabled();

            //pInspector->SetTargetObject(pTargetObj);

            //ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Add Component"))
        {
            Inspector* pInspector = CImGuiManager::GetInst()->FindEditorUI<Inspector>("Inspector");
            CGameObject* pTargetObj = pInspector->GetTargetObject();

            ImGui::BeginDisabled(!pTargetObj);

            for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
            {
                if (ImGui::MenuItem(COMPONENT_TYPE_STRING[i]))
                {
                    if (nullptr != pTargetObj)
                    {
                        switch (COMPONENT_TYPE(i))
                        {
                        case COMPONENT_TYPE::TRANSFORM:
                            pTargetObj->AddComponent(new CTransform);
                            break;
                        case COMPONENT_TYPE::CAMERA:
                            pTargetObj->AddComponent(new CCamera);
                            break;
                        case COMPONENT_TYPE::COLLIDER2D:
                            pTargetObj->AddComponent(new CCollider2D);
                            break;
                        case COMPONENT_TYPE::COLLIDER3D:

                            break;
                        case COMPONENT_TYPE::ANIMATOR2D:
                            pTargetObj->AddComponent(new CAnimator2D);
                            break;
                        case COMPONENT_TYPE::ANIMATOR3D:

                            break;
                        case COMPONENT_TYPE::LIGHT2D:
                            pTargetObj->AddComponent(new CLight2D);
                            break;
                        case COMPONENT_TYPE::LIGHT3D:
                            break;
                        case COMPONENT_TYPE::MESHRENDER:
                            pTargetObj->AddComponent(new CMeshRender);
                            break;
                        case COMPONENT_TYPE::DECAL:
                            break;
                        case COMPONENT_TYPE::PARTICLESYSTEM:
                            pTargetObj->AddComponent(new CParticleSystem);
                            break;
                        case COMPONENT_TYPE::TILEMAP:
                            pTargetObj->AddComponent(new CTileMap);
                            break;
                        case COMPONENT_TYPE::LANDSCAPE:
                            break;
                        }
                    }
                }
            }

            ImGui::EndDisabled();

            pInspector->SetTargetObject(pTargetObj);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Add Script"))
        {
            Inspector* pInspector = CImGuiManager::GetInst()->FindEditorUI<Inspector>("Inspector");
            CGameObject* pTargetObj = pInspector->GetTargetObject();

            ImGui::BeginDisabled(!pTargetObj);

            vector<wstring> vecScriptName;
            CScriptManager::GetScriptInfo(vecScriptName);

            for (size_t i = 0; i < vecScriptName.size(); i++)
            {
                string scriptName = ToString(vecScriptName[i]);

                if (ImGui::MenuItem(scriptName.c_str()))
                {
                    CScript* pNewScript = CScriptManager::GetScript(vecScriptName[i]);
                    pTargetObj->AddComponent(pNewScript);
                    pInspector->SetTargetObject(pTargetObj);
                    pInspector->SetFocus();
                }
            }

            ImGui::EndDisabled();

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void MenuUI::Asset()
{
    if (ImGui::BeginMenu("Asset"))
    {
        if (ImGui::MenuItem("Create Empty Material"))
        {
            // Material 생성 -> Asset Manager 등록
            Ptr<CMaterial> pMaterial = new CMaterial;

            wstring strMaterialKey = GetAssetDefaultName(L"material\\Default Material");
            strMaterialKey += L".mtrl";
            CAssetManager::GetInst()->AddAsset<CMaterial>(strMaterialKey, pMaterial);

            // Material 파일 저장
            wstring strFilePath = CPathManager::GetInst()->GetContentPath();
            strFilePath += strMaterialKey;
            pMaterial->Save(strFilePath);

            // 새로운 Asset 이 등록되었기 때문에, ContentUI 갱신
            ContentUI* pContent = CImGuiManager::GetInst()->FindEditorUI<ContentUI>("Content");
            pContent->RenewContent();
        }

        ImGui::Separator();

        ImGui::EndMenu();
    }
}

wstring MenuUI::GetAssetDefaultName(wstring baseName)
{
    baseName += L" %d";

    wchar_t szKey[255] = {};

    int i = 0;
    while (true)
    {
        wchar_t tempKey[255] = {};
        wcscpy_s(szKey, 255, baseName.c_str());
        swprintf_s(szKey, 255, szKey, i++);
        wcscpy_s(tempKey, 255, szKey);
        wcscat_s(tempKey, L".mtrl");

        if (CAssetManager::GetInst()->FindAsset<CMaterial>(tempKey) == nullptr)
        {
            break;
        }
    }

    return szKey;
}
