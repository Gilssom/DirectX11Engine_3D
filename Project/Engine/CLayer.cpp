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
	// �ֻ��� ������Ʈ���� �ٷ� ó���� ���־�� �Ѵ�.
	// ( �ڽ� ������Ʈ�� ���޾Ƽ� ���� ó���� �����ؾ� �ϱ� ������ )
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
	// �ֻ��� �θ� ������Ʈ�� ���
	if (!object->GetParent())
	{
		m_vecParent.push_back(object);
	}
	
	// ������Ʈ�� �ڽ��� �����ϰ� �ִٸ�
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
			// �ڽı��� ���̾� �̵��� �ƴϸ�, ���̾�� �ҼӵǴ� �θ� ������Ʈ�� Layer Index üũ
			// �ڽ� ������Ʈ�� Layer Index �� -1 �̸�(���Ҽ�), �̷� ���� ���������� Layer Index üũ
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