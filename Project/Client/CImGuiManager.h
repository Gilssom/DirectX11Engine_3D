#pragma once

typedef void (*UI_CALLBACK)(void);

// Editor UI 의 멤버함수이면서, 반환 타입은 UINT, 인자는 3가지 종류
class EditorUI;
typedef UINT(EditorUI::* UI_DELEGATE)(void);
typedef UINT(EditorUI::* UI_DELEGATE1)(DWORD_PTR);
typedef UINT(EditorUI::* UI_DELEGATE2)(DWORD_PTR, DWORD_PTR);


class CImGuiManager : public CSingleton<CImGuiManager>
{
    SINGLE(CImGuiManager);
private:
    HWND                    m_hMainHwnd;
    map<string, EditorUI*>  m_mapUI;
    bool                    m_bDemo;
    HANDLE                  m_hNotify;

public:
    template<typename T>
    T* FindEditorUI(const string& strName);
    
public:
    int Init(HWND hwnd);
    void Tick();

private:
    void CreateEditorUI();
    void ObserveContent();
    
};

template<typename T>
inline T* CImGuiManager::FindEditorUI(const string& strName)
{
    map<string, EditorUI*>::iterator iter = m_mapUI.find(strName);

    if (iter == m_mapUI.end())
        return nullptr;

    // 최상위 EditorUI 를 dynamic_cast 로 자신이 원하는 Type 의 하위 Class 를 찾아줌
    T* pUI = dynamic_cast<T*>(iter->second);

    return pUI;
}
