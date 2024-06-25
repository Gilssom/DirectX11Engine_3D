#pragma once
#include "Singletone.h"


enum class KEY
{
    W, S, A, D,
    Z, X, C, V,

    R, T, Y, U, I, O, P, F,

    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,

    NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,

    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    LEFT,
    RIGHT,
    UP,
    DOWN,

    LBTN,
    RBTN,

    ENTER,
    ESC,
    SPACE,
    LSHIFT,
    ALT,
    CTRL,

    KEY_END,
};

enum class KEY_STATE
{
    TAP,        // 눌렀다
    PRESSED,    // 눌린 상태
    RELEASED,   // 눌렀다가 뗀 상태
    NONE,
};

struct KeyInfo
{
    KEY_STATE   State;
    bool        PrevPressed;
};

class CKeyManager : public CSingleton<CKeyManager>
{
    SINGLE(CKeyManager)

private:
    vector<KeyInfo> m_VecKey;
    Vec2            m_CurMousePos;
    Vec2            m_PrevMousePos; // 이전 마우스 좌표
    Vec2            m_DragDir;

public:
    void Init();
    void Tick();

public:
    KEY_STATE GetKeyState(KEY _key) { return m_VecKey[(UINT)_key].State; }

    Vec2 GetMousePos() { return m_CurMousePos; };
    Vec2 GetPrevMousePos() { return m_PrevMousePos; };
    Vec2 GetMouseDrag() { return m_DragDir; };
};

