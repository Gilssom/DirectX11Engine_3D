#pragma once

// 데이터 영역 메모리 : 전역 변수
// 프로세스 시작할 때 초기화
// 프로세스 종료 시 해제
// 프로세스가 실행 도중 상시 유지되는 메모리

class CEngine : public CSingleton<CEngine>
{
	SINGLE(CEngine) // 매크로 구현

private:
	HWND	m_hMainHwnd;
	Vec2	m_Resolution;

public:
	HWND GetMainWnd() { return m_hMainHwnd; }
	int Init(HWND hwnd, Vec2 resolution, PREFAB_SAVE_FUNC save, PREFAB_LOAD_FUNC load);
	void Progress();
};