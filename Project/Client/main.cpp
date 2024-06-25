// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"

#include "framework.h"
#include "Client.h"

#include <Windows.h>

#include <Engine//global.h>
#include <Engine//CEngine.h>
#include <Engine//CDevice.h>
#include <crtdbg.h>


// Game Release Mode 면 Game 에 필요한 부분만 나타날 수 있게 설정 (주석 풀기)
//#define GAME_RELEASE


#ifdef _DEBUG
#pragma comment(lib, "Engine//Engine_d.lib")
#else
#pragma comment(lib, "Engine//Engine.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "Scripts//Scripts_d.lib")
#else
#pragma comment(lib, "Scripts//Scripts.lib")
#endif

#include "CTestLevel.h"
#include "CEditorManager.h"
#include "CImGuiManager.h"
#include "ImGui\\imgui.h"
#include "CLevelSaveLoad.h"

#include <Scripts\CUIManager.h>


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
//WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
//WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MyRegisterClass(hInstance);

    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    // 커널 오브젝트
    // 윈도우 생성 후 핸들값 반환
    HWND hWnd = CreateWindowW(L"Test", L"MyGame", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    // 윈도우 생성에 문제가 있으면 프로그램 종료
    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, true);
    UpdateWindow(hWnd);

    // CEngine 초기화
    if (FAILED(CEngine::GetInst()->Init(hWnd, Vec2(1280, 768)
        , (PREFAB_SAVE_FUNC)&CLevelSaveLoad::SaveGameObject
        , (PREFAB_LOAD_FUNC)&CLevelSaveLoad::LoadGameObject)))
    {
        MessageBox(nullptr, L"엔진 초기화 실패", L"엔진 초기화 실패", MB_OK);
        return 0;
    }

#ifndef GAME_RELEASE
    // Editor 초기화
    CEditorManager::GetInst()->Init();

    // ImGui 초기화
    if (FAILED(CImGuiManager::GetInst()->Init(hWnd)))
    {
        MessageBox(nullptr, L"ImGui 초기화 실패", L"ImGui 초기화 실패", MB_OK);
        return 0;
    }

    // 임시 Level 생성
    CTestLevel::CreateTestLevel();
#endif


    // 단축키 테이블 ( 그동안 Alt 누르면 프로그램이 일시정지 되었던 이유 ) 리소스 뷰 Accelerator 확인
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;

    // 기본 메시지 루프입니다:

    // 윈도우 메세지가 없으면 반환되지 않음
    /*while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }*/
    // 게임 구조와 맞지 않음

    while (true)
    {
        // PeekMessage :: 메세지가 있던 없던 함수가 무조건 반환됨. 그래서 while 문의 조건에 성립하지 않음
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // Game Engine 실행
            // CEngine::Progress() 한번 수행 ==> 1 프레임
            CEngine::GetInst()->Progress();

#ifndef GAME_RELEASE
            // Editor 실행
            CEditorManager::GetInst()->Tick();

            // ImGui 실행
            CImGuiManager::GetInst()->Tick();
#endif


            // Present (모든 Render 가 끝나고 Present 가 진행되어야 함)
            CDevice::GetInst()->Present();
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;//MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName  = L"Test";
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
