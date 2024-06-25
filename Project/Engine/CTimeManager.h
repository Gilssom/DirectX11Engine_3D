#pragma once
#include "Singletone.h"

class CTimeManager : public CSingleton<CTimeManager>
{
    SINGLE(CTimeManager)

private:
    LARGE_INTEGER   m_llFrequency;  // 초당 카운트 수
    LARGE_INTEGER   m_llCurCount;   // 현재 카운트 수
    LARGE_INTEGER   m_llPrevCount;  // 이전 프레임 카운트 수

    // System 의 DT 와 Game 의 DT 두개로 나뉘어서 활용을 해야한다.
    float           m_DT;           // Delta Time : 1 Frame 동작 하는데 걸린 시간 , Level Content 전용
    float           m_EngineDT;     // Delta Time : 1 Frame 동작 하는데 걸린 시간 , System 전용

    float           m_Time;         // 프로세스 실행 이후 누적 시간
    float           m_EngineTime;   // 프로세스 실행 이후 누적 시간 (레벨 상태에 영향을 받지 않음)
    float           m_AccTime;      // 1초를 체크하기 위한 시간 누적 변수

    UINT            m_FrameCount;

public:
    void Init();
    void Tick();

public:
    float GetDeltaTime() { return m_DT; }
    float GetDeltaTime_Engine() { return m_EngineDT; }
};

