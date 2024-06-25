#pragma once
#include "Singletone.h"

class CTimeManager : public CSingleton<CTimeManager>
{
    SINGLE(CTimeManager)

private:
    LARGE_INTEGER   m_llFrequency;  // �ʴ� ī��Ʈ ��
    LARGE_INTEGER   m_llCurCount;   // ���� ī��Ʈ ��
    LARGE_INTEGER   m_llPrevCount;  // ���� ������ ī��Ʈ ��

    // System �� DT �� Game �� DT �ΰ��� ����� Ȱ���� �ؾ��Ѵ�.
    float           m_DT;           // Delta Time : 1 Frame ���� �ϴµ� �ɸ� �ð� , Level Content ����
    float           m_EngineDT;     // Delta Time : 1 Frame ���� �ϴµ� �ɸ� �ð� , System ����

    float           m_Time;         // ���μ��� ���� ���� ���� �ð�
    float           m_EngineTime;   // ���μ��� ���� ���� ���� �ð� (���� ���¿� ������ ���� ����)
    float           m_AccTime;      // 1�ʸ� üũ�ϱ� ���� �ð� ���� ����

    UINT            m_FrameCount;

public:
    void Init();
    void Tick();

public:
    float GetDeltaTime() { return m_DT; }
    float GetDeltaTime_Engine() { return m_EngineDT; }
};

