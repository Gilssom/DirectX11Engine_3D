#include "pch.h"
#include "CTimeManager.h"

#include "CEngine.h"
#include "CLevelManager.h"
#include "CLevel.h"

CTimeManager::CTimeManager()
	: m_llFrequency{}
	, m_llCurCount{}
	, m_llPrevCount{}
	, m_DT(0.f)
	, m_Time(0.f)
	, m_EngineTime(0.f)
	, m_AccTime(0.f)
	, m_FrameCount(0)
{

}

CTimeManager::~CTimeManager()
{

}

void CTimeManager::Init()
{
	// 초당 카운트 수
	QueryPerformanceFrequency(&m_llFrequency);
	QueryPerformanceCounter(&m_llCurCount);
	m_llPrevCount = m_llCurCount;
}

void CTimeManager::Tick()
{
	QueryPerformanceCounter(&m_llCurCount);

	// 프레임과 프레임의 간격 시간 (System Delta Time 도 동일하게 계산)ㄴ
	m_EngineDT = m_DT = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequency.QuadPart;

	CLevel* pLevel = CLevelManager::GetInst()->GetCurrentLevel();

	// Level 이 Play 가 아니라면 기본 DT 는 0 으로 초기화
	if (pLevel == nullptr || pLevel->GetState() != LEVEL_STATE::PLAY)
	{
		m_DT = 0.f;
	}

	m_llPrevCount = m_llCurCount;

	// 누적 시간
	m_Time += m_DT;
	m_EngineTime += m_EngineDT;

	// m_AccTime는 1초만 세기 위함
	m_AccTime += m_DT;
	++m_FrameCount;

	if (1.f < m_AccTime)
	{
		HWND hMainWnd = CEngine::GetInst()->GetMainWnd();	
		wchar_t szText[255] = {};
		swprintf_s(szText, L"FPS : %d,  DeltaTime : %f", m_FrameCount, m_DT);
		SetWindowText(hMainWnd, szText);

		m_AccTime -= 1.f;
		m_FrameCount = 0;
	}

	// Global Time
	g_GlobalData.DeltaTime = m_DT;
	g_GlobalData.Time += m_DT;

	g_GlobalData.DeltaTimeEngine = m_EngineDT;
	g_GlobalData.TimeEngine = m_EngineTime;
}