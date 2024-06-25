#pragma once
#include "CEntity.h"

class CLayer;
class CGameObject;

class CLevel : public CEntity
{
private:
	CLayer*			m_arrLayer[MAX_LAYER];
	LEVEL_STATE		m_State;
	CSound*			m_BGM;
	bool			m_FightLevel;

public:
	void Init();
	void Tick();
	void FinalTick();
	void RegisterClear();

public:
	void AddObject(UINT layerIdx, CGameObject* object, bool childMove = false);
	CLayer* GetLayer(int layerIdx) { return m_arrLayer[layerIdx]; }
	LEVEL_STATE GetState() { return m_State; }

	// Ư�� �̸��� ���� GameObject �� �˻�(���� ���� ����� Object ��ȯ)
	CGameObject* FindObjectByName(const wstring& name);

	// Ư�� �̸��� ���� GameObject �� ��� �˻�
	void FindObjectByName(const wstring& name, vector<CGameObject*>& vecObject );

	// Ư�� Component �� ���� GameObject �� �˻�(���� ���� ����� Object ��ȯ)
	CGameObject* FindObjectByComponent(COMPONENT_TYPE type, const wstring& name);

	// Ư�� Component �� ���� GameObject �� ��� �˻�
	void FindObjectByComponent(COMPONENT_TYPE type, vector<CGameObject*>& vecObject);

	// ���� Level �� BGM ��� �� ��ȯ
	void SetBGM(CSound* bgm) { m_BGM = bgm; }
	CSound* GetBGM() { return m_BGM; }

	// ���� Level �� Ư�� ��
	void SetFightLevel(bool isFight) { m_FightLevel = isFight; }
	bool GetFightLevel() { return m_FightLevel; }

private:
	void ChangeState(LEVEL_STATE nextState);

public:
	CLONE_DISABLE(CLevel);

public:
	CLevel();
	~CLevel();

	friend class CTaskManager;
};

