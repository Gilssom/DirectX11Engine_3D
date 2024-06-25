#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderManager.h"
#include "CLevelManager.h"

CLevel::CLevel()
	: m_arrLayer{}
	, m_State(LEVEL_STATE::STOP)
	, m_BGM(nullptr)
	, m_FightLevel(false)
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i] = new CLayer;
		m_arrLayer[i]->m_LayerIdx = i;
	}
}

CLevel::~CLevel()
{
	Safe_Del_Array(m_arrLayer);
}

void CLevel::Init()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->Init();
	}
}

void CLevel::Tick()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->Tick();
	}
}

void CLevel::FinalTick()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->FinalTick();
	}
}

void CLevel::RegisterClear()
{
	// ���� �����ӿ� ��ϵ� Object ���� ��� ����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->Clear();
	}
}

void CLevel::AddObject(UINT layerIdx, CGameObject* object, bool childMove)
{
	m_arrLayer[layerIdx]->AddObject(object, childMove);
}

void CLevel::ChangeState(LEVEL_STATE nextState)
{
	// Level �� Stop �̳� Pause ���·� ��ȯ�Ǹ� Editor Mode �� ��ȯ
	if (nextState == LEVEL_STATE::STOP || nextState == LEVEL_STATE::PAUSE)
	{
		CRenderManager::GetInst()->ChangeRenderMode(RENDER_MODE::EDITOR);

		if(m_BGM)
			m_BGM->Stop();
	}

	// ������ ���·��� ��ȯ�� �߻��� ��� return
	if (m_State == nextState)
		return;

	// ���� Stop �����ε� Pause ���·��� ��ȯ�� �߻��� ���
	assert(!(m_State == LEVEL_STATE::STOP && nextState == LEVEL_STATE::PAUSE));

	// ���� Stop �����ε� Play ���·��� ��ȯ�� �߻��� ���
	if (m_State == LEVEL_STATE::STOP && nextState == LEVEL_STATE::PLAY)
	{
		Init();

		LevelInfo info = CLevelManager::GetInst()->GetCurLevel();

		if (m_BGM && info.bgm != L"")
			m_BGM->Play(0, 0.3f, true);
	}

	// Stop �Ǵ� Pause ���� Play �� ��ȯ�� �� Render Mode �� Play Mode �� ����
	if ((m_State == LEVEL_STATE::STOP || m_State == LEVEL_STATE::PAUSE) && nextState == LEVEL_STATE::PLAY)
	{
		CRenderManager::GetInst()->ChangeRenderMode(RENDER_MODE::PLAY);
	}

	m_State = nextState;
}

CGameObject* CLevel::FindObjectByName(const wstring& name)
{
	for (size_t i = 0; i < MAX_LAYER; i++)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();

		for (size_t j = 0; j < vecObjects.size(); j++)
		{
			if (vecObjects[j]->GetName() == name)
			{
				return vecObjects[j];
			}
		}
	}

	return nullptr;
}

void CLevel::FindObjectByName(const wstring& name, vector<CGameObject*>& vecObject)
{
	for (size_t i = 0; i < MAX_LAYER; i++)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();

		for (size_t j = 0; j < vecObjects.size(); j++)
		{
			if (vecObjects[j]->GetName() == name)
			{
				vecObject.push_back(vecObjects[j]);
			}
		}
	}
}

CGameObject* CLevel::FindObjectByComponent(COMPONENT_TYPE type, const wstring& name)
{
	for (size_t i = 0; i < MAX_LAYER; i++)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();

		for (size_t j = 0; j < vecObjects.size(); j++)
		{
			if (vecObjects[j]->GetComponent(type))
			{
				return vecObjects[j];
			}
		}
	}

	return nullptr;
}

void CLevel::FindObjectByComponent(COMPONENT_TYPE type, vector<CGameObject*>& vecObject)
{
	for (size_t i = 0; i < MAX_LAYER; i++)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();

		for (size_t j = 0; j < vecObjects.size(); j++)
		{
			if (vecObjects[j]->GetComponent(type))
			{
				vecObject.push_back(vecObjects[j]);
			}
		}
	}
}
