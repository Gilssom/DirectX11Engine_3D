#pragma once

// ������ ���� �޸� : ���� ����
// ���μ��� ������ �� �ʱ�ȭ
// ���μ��� ���� �� ����
// ���μ����� ���� ���� ��� �����Ǵ� �޸�

class CEngine : public CSingleton<CEngine>
{
	SINGLE(CEngine) // ��ũ�� ����

private:
	HWND	m_hMainHwnd;
	Vec2	m_Resolution;

public:
	HWND GetMainWnd() { return m_hMainHwnd; }
	int Init(HWND hwnd, Vec2 resolution, PREFAB_SAVE_FUNC save, PREFAB_LOAD_FUNC load);
	void Progress();
};