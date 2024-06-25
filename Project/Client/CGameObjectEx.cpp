#include "pch.h"
#include "CGameObjectEx.h"

#include <Engine\components.h>

void CGameObjectEx::FinalTick()
{
	// Component Final Tick ȣ��
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (GetComponent(COMPONENT_TYPE(i)) != nullptr)
			GetComponent(COMPONENT_TYPE(i))->FinalTick();
	}

	// �ڽ� ������Ʈ Final Tick ȣ��
	const vector<CGameObject*>& vecChild = GetChildren();

	for (size_t i = 0; i < vecChild.size(); i++)
	{
		vecChild[i]->FinalTick();
	}
}