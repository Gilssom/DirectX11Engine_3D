#pragma once

typedef void (*UI_CALLBACK)(void);

// Editor UI �� ����Լ��̸鼭, ��ȯ Ÿ���� UINT, ���ڴ� 3���� ����
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

    // �ֻ��� EditorUI �� dynamic_cast �� �ڽ��� ���ϴ� Type �� ���� Class �� ã����
    T* pUI = dynamic_cast<T*>(iter->second);

    return pUI;
}
