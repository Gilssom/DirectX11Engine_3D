#pragma once
#include "Singletone.h"

// Update 를 할 때 예를 들어 갑자기 중간에 오브젝트가 생성 되거나 삭제되어야 할 때,
// 다른 영향을 끼치는 함수가 들어오면 내부적으로 문제가 생길 수 있다.
// 이를 방지하기 위해 만드는 Task Manager ( 프레임 동기화 )
// 한 프레임에서 누가 있고 없고의 판단을 다르게 하게 될 수도 있음
// 그래서 이전의 모든 Tick , Final Tick 을 모두 실행 되고,
// 다음번 프레임부터 적용될 수 있게 관리를 해야됨.

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

// 오브젝트 삭제 처리
// 1. 오브젝트를 바로 삭제시키면 해당 프레임에서 
//    그 오브젝트의 주소를 가지고 있는 오브젝트들이 문제가 생김.
// 2. 먼저 삭제할 오브젝트를 Dead 처리를 함
// 3. 그 오브젝트를 가르키고 있는 곳에서 Dead 상황에 대한 예외 처리를 둠
// 4. 그 다음 Task 에서 해당 Dead 오브젝트를 삭제시킴