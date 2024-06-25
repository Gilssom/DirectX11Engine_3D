#pragma once
#include "Singletone.h"

// Update �� �� �� ���� ��� ���ڱ� �߰��� ������Ʈ�� ���� �ǰų� �����Ǿ�� �� ��,
// �ٸ� ������ ��ġ�� �Լ��� ������ ���������� ������ ���� �� �ִ�.
// �̸� �����ϱ� ���� ����� Task Manager ( ������ ����ȭ )
// �� �����ӿ��� ���� �ְ� ������ �Ǵ��� �ٸ��� �ϰ� �� ���� ����
// �׷��� ������ ��� Tick , Final Tick �� ��� ���� �ǰ�,
// ������ �����Ӻ��� ����� �� �ְ� ������ �ؾߵ�.

class CTaskManager : public CSingleton<CTaskManager>
{
	SINGLE(CTaskManager);

private:
	vector<tTask>		 m_vecTasks;
	vector<CGameObject*> m_vecDead;

	bool				 m_ObjectEvent;

public:
	bool IsObjectsChanged() { return m_ObjectEvent; }

public:
	void Tick();
	void AddTask(const tTask& task) { m_vecTasks.push_back(task); }

private:
	void ClearDeadObject();
	void ExecuteTask(tTask& task);
	void ClearEvent() { m_ObjectEvent = false; }

};

// ������Ʈ ���� ó��
// 1. ������Ʈ�� �ٷ� ������Ű�� �ش� �����ӿ��� 
//    �� ������Ʈ�� �ּҸ� ������ �ִ� ������Ʈ���� ������ ����.
// 2. ���� ������ ������Ʈ�� Dead ó���� ��
// 3. �� ������Ʈ�� ����Ű�� �ִ� ������ Dead ��Ȳ�� ���� ���� ó���� ��
// 4. �� ���� Task ���� �ش� Dead ������Ʈ�� ������Ŵ