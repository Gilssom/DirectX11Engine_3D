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
	// 이전 프레임에 등록된 Object 들을 모두 제거
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
	// Level 이 Stop 이나 Pause 상태로 전환되면 Editor Mode 로 전환
	if (nextState == LEVEL_STATE::STOP || nextState == LEVEL_STATE::PAUSE)
	{
		CRenderManager::GetInst()->ChangeRenderMode(RENDER_MODE::EDITOR);

		if(m_BGM)
			m_BGM->Stop();
	}

	// 동일한 상태로의 전환이 발생한 경우 return
	if (m_State == nextState)
		return;

	// 현재 Stop 상태인데 Pause 상태로의 전환이 발생한 경우
	assert(!(m_State == LEVEL_STATE::STOP && nextState == LEVEL_STATE::PAUSE));

	// 현재 Stop 상태인데 Play 상태로의 전환이 발생한 경우
	if (m_State == LEVEL_STATE::STOP && nextState == LEVEL_STATE::PLAY)
	{
		Init();

		LevelInfo info = CLevelManager::GetInst()->GetCurLevel();

		if (m_BGM && info.bgm != L"")
			m_BGM->Play(0, 0.3f, true);
	}

	// Stop 또는 Pause 에서 Play 로 전환될 때 Render Mode 를 Play Mode 로 변경
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
