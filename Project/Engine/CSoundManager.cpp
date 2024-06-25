#include "pch.h"
#include "CSoundManager.h"

#include "CSound.h"

CSoundManager::CSoundManager()
	: m_pFMODSystem(nullptr)
{
}

CSoundManager::~CSoundManager()
{
	if (nullptr != m_pFMODSystem)
	{
		m_pFMODSystem->release();
		m_pFMODSystem = nullptr;
	}
}

void CSoundManager::Init()
{
	FMOD::System_Create(&m_pFMODSystem);

	if (nullptr == m_pFMODSystem)
	{
		assert(nullptr);
	}

	// 32개 채널 생성
	m_pFMODSystem->init(32, FMOD_DEFAULT, nullptr);
}

void CSoundManager::Tick()
{
	m_pFMODSystem->update();
}
