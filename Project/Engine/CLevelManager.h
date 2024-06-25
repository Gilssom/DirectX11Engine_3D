#pragma once
#include "Singletone.h" 

struct LevelInfo
{
	wstring name;
	wstring path;
	wstring bgm;
	bool isFight;
};

class CLevel;

class CLevelManager : public CSingleton<CLevelManager>
{
	SINGLE(CLevelManager)
private:
	CLevel* m_CurLevel;
	CSound* m_CurSound;

	vector<LevelInfo> m_Levels;
	size_t m_CurrentLevelIndex;

public:
	CLevel* GetCurrentLevel() { return m_CurLevel; }
	LevelInfo GetNextLevel();
	LevelInfo GetCurLevel() { return m_Levels[m_CurrentLevelIndex]; }

	void Reset() { m_CurrentLevelIndex = 0; }
	void ChangeNextLevel();
	void ChangeBgm(Ptr<CSound> sound);
	void GameEndEvent(Ptr<CSound> sound, int repeat);

public:
	void Init();
	void Tick();

private:
	void ChangeLevel(CLevel* nextLevel);

	friend class CTaskManager;
};

