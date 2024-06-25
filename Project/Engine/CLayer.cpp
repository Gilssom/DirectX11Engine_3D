#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()
{

}

CLayer::~CLayer()
{
	Safe_Del_Vector(m_vecParent);
}

void CLayer::Init()
{
	for (size_t i = 0; i < m_vecParent.size(); ++i)
	{
		m_vecParent[i]->Begin();
	}
}

void CLayer::Tick()
{
	for (size_t i = 0; i < m_vecParent.size(); i++)
	{
		m_vecParent[i]->Tick();
	}
}

void CLayer::FinalTick()
{
	// 최상위 오브젝트들은 바로 처리를 해주어야 한다.
	// ( 자식 오브젝트도 덩달아서 삭제 처리를 진행해야 하기 때문에 )
	vector<CGameObject*>::iterator iter = m_vecParent.begin();

	for (; iter != m_vecParent.end();)
	{
		(*iter)->FinalTick();

		if ((*iter)->IsDead())
		{
			iter = m_vecParent.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

void CLayer::AddObject(CGameObject* object, bool childMove)
{
	// 최상위 부모 오브젝트일 경우
	if (!object->GetParent())
	{
		m_vecParent.push_back(object);
	}
	
	// 오브젝트가 자식을 보유하고 있다면
	static list<CGameObject*> queue;
	queue.clear();
	queue.push_back(object);

	while (!queue.empty())
	{
		CGameObject* pObject = queue.front();
		queue.pop_front();

		if(childMove)
			pObject->m_LayerIdx = m_LayerIdx;
		else
		{
			// 자식까지 레이어 이동이 아니면, 레이어로 소속되는 부모 오브젝트만 Layer Index 체크
			// 자식 오브젝트가 Layer Index 가 -1 이면(무소속), 이럴 경우는 예외적으로 Layer Index 체크
			if(pObject == object || pObject->m_LayerIdx == -1)
				pObject->m_LayerIdx = m_LayerIdx;
		}

		vector<CGameObject*> vecChildren = pObject->GetChildren();

		for (size_t i = 0; i < vecChildren.size(); i++)
		{
			queue.push_back(vecChildren[i]);
		}
	}
}

void CLayer::DeRegisterParentObject(CGameObject* object)
{
	vector<CGameObject*>::iterator iter = m_vecParent.begin();

	for (iter; iter != m_vecParent.end(); ++iter)
	{
		if ((*iter) == object)
		{
			m_vecParent.erase(iter);
			return;
		}
	}

	assert(nullptr);
}