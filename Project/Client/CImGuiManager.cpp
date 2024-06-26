#include "pch.h"
#include "CImGuiManager.h"

#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_win32.h"
#include "ImGui\\imgui_impl_dx11.h"

#include <Engine\\CKeyManager.h>
#include <Engine\\CDevice.h>

#include "ParamUI.h"

#include "Inspector.h"
#include "ListUI.h"
#include "ContentUI.h"
#include "OutlinerUI.h"
#include "MenuUI.h"

#include "TileMapEditor.h"
#include "TileView.h"
#include "TileMapDetail.h"
#include "AtlasView.h"

#include "AnimationEditor.h"
#include "SpriteView.h"
#include "AnimPreView.h"
#include "AnimDetail.h"

CImGuiManager::CImGuiManager()
    : m_mapUI{}
    , m_hMainHwnd(nullptr)
    , m_bDemo(false)
    , m_hNotify(nullptr)
{

}

CImGuiManager::~CImGuiManager()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Delete UI
    Safe_Del_Map(m_mapUI);
}

int CImGuiManager::Init(HWND hwnd)
{
    m_hMainHwnd = hwnd;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsDrakular();
    //ImGui::StyleColorsLight();

    //struct FontInfo
    //{
    //    const wchar_t* path;
    //    float size;
    //};
    //
    //FontInfo fonts[] =
    //{
    //    {L"Fonts\\Roboto-Regular.ttf", 16.f},
    //    {L"Fonts\\NanumSquareNeoOTF-cBd.otf", 16.f}
    //};
    //
    //for (const auto& font : fonts)
    //{
    //    // ImGui Font 상대경로 설정
    //    wstring strFontPath = CPathManager::GetInst()->GetContentPath();
    //    strFontPath += font.path;
    //    size_t len = strFontPath.length();
    //    size_t convertedChars = 0;
    //    char* cstr = new char[len + 1];
    //    wcstombs_s(&convertedChars, cstr, len + 1, strFontPath.c_str(), len);
    //    io.Fonts->AddFontFromFileTTF(cstr, font.size, NULL);
    //    delete[] cstr;
    //}


    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }


    // Setup Platform/Renderer backends
    if (!ImGui_ImplWin32_Init(m_hMainHwnd))
        return E_FAIL;

    if (!ImGui_ImplDX11_Init(DEVICE, CONTEXT))
        return E_FAIL;

    CreateEditorUI();

    // Content Folder 감시 기능
    wstring strContentPath = CPathManager::GetInst()->GetContentPath();

    // 변경점이 감지된 순간 알림 기능 객체 (파일 및 폴더 이름 변경, 파일 추가 및 삭제)
    m_hNotify = FindFirstChangeNotification(strContentPath.c_str(), true
            , FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
            | FILE_ACTION_ADDED | FILE_ACTION_REMOVED);

	return S_OK;
}

void CImGuiManager::Tick()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Parameter UI ID 초기화
    ParamUI::ResetParamID();

    // Demo Window 활성화 비활성화
    if (KEY_TAP(KEY::F1)) { m_bDemo ? m_bDemo = false : m_bDemo = true; }
    if (m_bDemo) { ImGui::ShowDemoWindow(); }

    // Begin - End : Window 하나
    for (const auto& pair : m_mapUI)
    {
        if(pair.second->IsActive())
            pair.second->Tick();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    // Content 변경 알림 확인
    ObserveContent();
}

void CImGuiManager::CreateEditorUI()
{
    EditorUI* pUI = nullptr;

    pUI = new Inspector;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new ListUI;
    pUI->SetActive(false);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new ContentUI;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new OutlinerUI;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new MenuUI;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));


// =================
//  Tile Map Editor
// =================
    pUI = new TileView;
    pUI->SetActive(false);
    pUI->SetUIMove(false);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new AtlasView;
    pUI->SetActive(false);
    pUI->SetUIMove(false);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new TileMapDetail;
    pUI->SetActive(false);
    pUI->SetUIMove(false);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new TileMapEditor;
    pUI->SetActive(false);
    pUI->SetMenu(true);
    pUI->SetUIMove(true);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

// ==================
//  Animation Editor
// ==================
    pUI = new SpriteView;
    pUI->SetActive(false);
    pUI->SetUIMove(false);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new AnimPreView;
    pUI->SetActive(false);
    pUI->SetUIMove(false);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new AnimDetail;
    pUI->SetActive(false);
    pUI->SetUIMove(false);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new AnimationEditor;
    pUI->SetActive(false);
    pUI->SetMenu(true);
    pUI->SetUIMove(false);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));
}

void CImGuiManager::ObserveContent()
{
    DWORD dwWaitStatus = WaitForSingleObject(m_hNotify, 0);

    // 알림 활성화 체크 ( = WAIT_OBJECT_0 )
    if (dwWaitStatus == WAIT_OBJECT_0)
    {
        // 만약 파일 및 폴더에 변경점이 생겼다면 재로딩 및 목록 갱신
        ContentUI* pContentUI = FindEditorUI<ContentUI>("Content");
        pContentUI->ReloadContent();
        pContentUI->RenewContent();

        FindNextChangeNotification(m_hNotify);
    }
}