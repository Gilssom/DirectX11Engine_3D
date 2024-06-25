#include "pch.h"
#include "CKeyManager.h"

#include "CEngine.h"

UINT g_KeyValue[(UINT)KEY::KEY_END]
=
{
    'W', 'S', 'A', 'D',
    'Z', 'X', 'C', 'V',

    'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'F',

    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',

    VK_NUMPAD0,
    VK_NUMPAD1, 
    VK_NUMPAD2,
    VK_NUMPAD3,
    VK_NUMPAD4,
    VK_NUMPAD5,
    VK_NUMPAD6,
    VK_NUMPAD7,
    VK_NUMPAD8,
    VK_NUMPAD9,

    VK_F1,
    VK_F2,
    VK_F3,
    VK_F4,
    VK_F5,
    VK_F6,
    VK_F7,
    VK_F8,
    VK_F9,
    VK_F10,
    VK_F11,
    VK_F12,

    VK_LEFT,
    VK_RIGHT,
    VK_UP,
    VK_DOWN,

    VK_LBUTTON,
    VK_RBUTTON,

    VK_RETURN,
    VK_ESCAPE,
    VK_SPACE,
    VK_LSHIFT,
    VK_LMENU,
    VK_LCONTROL
};

CKeyManager::CKeyManager()
{

}

CKeyManager::~CKeyManager()
{

}

void CKeyManager::Init()
{
	for (UINT i = 0; i < (UINT)KEY::KEY_END; ++i)
	{
		KeyInfo info = {};
		info.State = KEY_STATE::NONE;
		info.PrevPressed = false;

		m_VecKey.push_back(info);
	}
}

void CKeyManager::Tick()
{
    if (GetFocus() == nullptr)
    {
        for (size_t i = 0; i < m_VecKey.size(); i++)
        {
            if (KEY_STATE::TAP == m_VecKey[i].State || KEY_STATE::PRESSED == m_VecKey[i].State)
            {
                m_VecKey[i].State = KEY_STATE::RELEASED;
            }
            else if (KEY_STATE::RELEASED == m_VecKey[i].State)
            {
                m_VecKey[i].State = KEY_STATE::NONE;
            }

            m_VecKey[i].PrevPressed = false;
        }
    }
    else
    {
        for (size_t i = 0; i < m_VecKey.size(); ++i)
        {
            // Key 가 눌렸다.
            if (GetAsyncKeyState(g_KeyValue[i]) & 0x8001)
            {
                // 이전에 안눌려있었다.
                if (!m_VecKey[i].PrevPressed)
                {
                    // 처음 누른 상태
                    m_VecKey[i].State = KEY_STATE::TAP;
                }
                else
                {
                    // 계속 누르고 있는 상태
                    m_VecKey[i].State = KEY_STATE::PRESSED;
                }

                m_VecKey[i].PrevPressed = true;
            }

            else
            {
                // 해당 Key 가 안눌려있다.
                if (m_VecKey[i].PrevPressed)
                {
                    // 이전 Frame 에서는 눌려있었다. = 뗀 상태
                    m_VecKey[i].State = KEY_STATE::RELEASED;
                }
                else
                {
                    // 이전에도 안눌려 있었고 지금도 안눌려있다.
                    m_VecKey[i].State = KEY_STATE::NONE;
                }

                m_VecKey[i].PrevPressed = false;
            }
        }

        // 마우스 좌표 갱신
        m_PrevMousePos = m_CurMousePos;

        POINT ptMouse = {};
        GetCursorPos(&ptMouse);
        ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMouse);
        m_CurMousePos = Vec2((float)ptMouse.x, (float)ptMouse.y);

        m_DragDir = m_CurMousePos - m_PrevMousePos; // 마우스의 이동량 = 방향 정보
        m_DragDir.y *= -1.f;
        m_DragDir.Normalize();
    }
}
