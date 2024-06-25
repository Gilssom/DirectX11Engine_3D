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
            // Key �� ���ȴ�.
            if (GetAsyncKeyState(g_KeyValue[i]) & 0x8001)
            {
                // ������ �ȴ����־���.
                if (!m_VecKey[i].PrevPressed)
                {
                    // ó�� ���� ����
                    m_VecKey[i].State = KEY_STATE::TAP;
                }
                else
                {
                    // ��� ������ �ִ� ����
                    m_VecKey[i].State = KEY_STATE::PRESSED;
                }

                m_VecKey[i].PrevPressed = true;
            }

            else
            {
                // �ش� Key �� �ȴ����ִ�.
                if (m_VecKey[i].PrevPressed)
                {
                    // ���� Frame ������ �����־���. = �� ����
                    m_VecKey[i].State = KEY_STATE::RELEASED;
                }
                else
                {
                    // �������� �ȴ��� �־��� ���ݵ� �ȴ����ִ�.
                    m_VecKey[i].State = KEY_STATE::NONE;
                }

                m_VecKey[i].PrevPressed = false;
            }
        }

        // ���콺 ��ǥ ����
        m_PrevMousePos = m_CurMousePos;

        POINT ptMouse = {};
        GetCursorPos(&ptMouse);
        ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMouse);
        m_CurMousePos = Vec2((float)ptMouse.x, (float)ptMouse.y);

        m_DragDir = m_CurMousePos - m_PrevMousePos; // ���콺�� �̵��� = ���� ����
        m_DragDir.y *= -1.f;
        m_DragDir.Normalize();
    }
}
