#pragma once
#include "Singletone.h"

#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>

#ifdef _DEBUG
#pragma comment(lib, "FMOD/fmodL64_vc.lib")
#else
#pragma comment(lib, "FMOD/fmod64_vc.lib")
#endif

class CSoundManager : public CSingleton<CSoundManager>
{
    SINGLE(CSoundManager);

private:
    FMOD::System*   m_pFMODSystem;  // FMOD 관리자 클래스
    //Ptr<CSound>     m_CurSound;

public:
    FMOD::System* GetFMODSystem() { return m_pFMODSystem; }

   // void SetCurSound(Ptr<CSound> curSound) { m_CurSound = curSound; }
   // Ptr<CSound> GetCurSound() { return m_CurSound; }

public:
    void Init();
    void Tick();
};

