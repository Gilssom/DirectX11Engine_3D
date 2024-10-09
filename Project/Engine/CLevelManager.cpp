#include "pch.h"
#include "CLevelManager.h"
#include "CAssetManager.h"
#include "CCollisionManager.h"

#include "CLevel.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CLight2D.h"

#include "..\Client\CLevelSaveLoad.h"

CLevelManager::CLevelManager()
	: m_CurLevel(nullptr)
	, m_CurSound(nullptr)
	, m_Levels{}
	, m_CurrentLevelIndex(0)
{
	// 현재 존재하는 모든 레벨 관리
	wstring path = CPathManager::GetInst()->GetContentPath();
	m_Levels.push_back({ L"ElvenGard_0", path + L"Level\\ElvenGard_0.lv", L"Sound\\elven_guard_old.wav", false });
	m_Levels.push_back({ L"ElvenGard_1", path + L"Level\\ElvenGard_1.lv", L"" , false });
	m_Levels.push_back({ L"MirkWood_0",	 path + L"Level\\MirkWood_0.lv",  L"Sound\\mirkwood.ogg", true });
	m_Levels.push_back({ L"MirkWood_1",	 path + L"Level\\MirkWood_1.lv",  L"" , true });
	m_Levels.push_back({ L"MirkWood_2",	 path + L"Level\\MirkWood_2.lv",  L"Sound\\mirkwood_boss.ogg" , true });
}

CLevelManager::~CLevelManager()
{
	if (m_CurLevel != nullptr)
	{
		delete m_CurLevel;
	}
}

void CLevelManager::Init()
{
	
}

void CLevelManager::Tick()
{
	if (m_CurLevel != nullptr)
	{
		// Level 이 Play 상태가 아니라면 Tick 은 호출 금지
		if (m_CurLevel->GetState() == LEVEL_STATE::PLAY)
		{
			m_CurLevel->Tick();
		}

		// Level 이 정지되더라도 Editor 에서도 볼 수 있어야 한다.
		// Final Tick 은 Rendering 을 진행하는 과정이기 때문에 남겨둔다.
		m_CurLevel->RegisterClear();
		m_CurLevel->FinalTick();
	}
}

void CLevelManager::ChangeLevel(CLevel* nextLevel)
{
	if (m_CurLevel != nullptr)
	{
		delete m_CurLevel;
		m_CurLevel = nullptr;
	}

	m_CurLevel = nextLevel;
}

LevelInfo CLevelManager::GetNextLevel()
{
	if (m_CurrentLevelIndex < m_Levels.size())
	{
		return m_Levels[++m_CurrentLevelIndex];
	}

	return { L"", L"", L"" };
}

void CLevelManager::ChangeNextLevel()
{
	LevelInfo nextLevel = GetNextLevel();
	m_CurSound = m_CurLevel->GetBGM();

	if (!nextLevel.path.empty())
	{
		CLevel* pNextLevel = CLevelSaveLoad::LoadLevel(nextLevel.path);

		if (nextLevel.bgm != L"")
		{
			m_CurSound->Stop();
			m_CurSound = nullptr;

			m_CurSound = CAssetManager::GetInst()->FindAsset<CSound>(nextLevel.bgm).Get();
			m_CurSound->Play(0, 0.3f, true);
		}

		pNextLevel->SetName(nextLevel.name);
		pNextLevel->SetBGM(m_CurSound);
		pNextLevel->SetFightLevel(nextLevel.isFight);

		pNextLevel->GetLayer(0)->SetName(L"Default");
		pNextLevel->GetLayer(1)->SetName(L"BackGround");
		pNextLevel->GetLayer(2)->SetName(L"Back Object");
		pNextLevel->GetLayer(3)->SetName(L"Monster");
		pNextLevel->GetLayer(4)->SetName(L"Player");
		pNextLevel->GetLayer(5)->SetName(L"Player Attack");
		pNextLevel->GetLayer(6)->SetName(L"Front Object");
		pNextLevel->GetLayer(7)->SetName(L"Gate Portal");
		pNextLevel->GetLayer(9)->SetName(L"Wall");

		CCollisionManager::GetInst()->LayerCheck(2, 4);

		CCollisionManager::GetInst()->LayerCheck(4, 3);
		CCollisionManager::GetInst()->LayerCheck(4, 6);
		CCollisionManager::GetInst()->LayerCheck(4, 7);
		
		CCollisionManager::GetInst()->LayerCheck(5, 3);
		
		CCollisionManager::GetInst()->LayerCheck(9, 3);
		CCollisionManager::GetInst()->LayerCheck(9, 4);

		ChangeLevelRegister(pNextLevel, LEVEL_STATE::PLAY);
	}
}

void CLevelManager::ChangeBgm(Ptr<CSound> sound)
{
	if (m_CurSound != nullptr)
	{
		m_CurSound->Stop();
		m_CurSound = nullptr;
	}

	m_CurSound = sound.Get();
	m_CurSound->Play(1, 0.3f, true);
}

void CLevelManager::GameEndEvent(Ptr<CSound> sound, int repeat)
{
	m_CurSound->Stop();
	m_CurSound = nullptr;

	m_CurSound = sound.Get();
	m_CurSound->Play(repeat, 0.3f, true);

	CGameObject* pLightObject = m_CurLevel->FindObjectByName(L"Directional Light");
	pLightObject->Light2D()->SetDiffuse(Vec3(0.3f, 0.3f, 0.3f));
}