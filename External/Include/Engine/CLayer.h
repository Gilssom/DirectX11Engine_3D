#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer : public CEntity
{
private:
	vector<CGameObject*> m_vecParent;  // 현재 Layer 에 소속되어 있는 최상위 부모 오브젝트만 관리
	vector<CGameObject*> m_vecObjects; // 현재 Layer 에 소속되어 있는 모든 오브젝트들만 관리
	int					 m_LayerIdx;

public:
	void Init();
	void Tick();
	void FinalTick();


public:
	// childMove = object 의 자식들도 해당 Layer 로 같이 옮길 것인지, 옮기지 않을 것인지
	void AddObject(CGameObject* object, bool childMove = false);

	void RegisterObject(CGameObject* object) { m_vecObjects.push_back(object); }
	void Clear() { m_vecObjects.clear(); }

	const vector<CGameObject*>& GetParentObjects() { return m_vecParent; }
	const vector<CGameObject*>& GetObjects() { return m_vecObjects; }

	void DeRegisterParentObject(CGameObject* object);

	CLONE_DISABLE(CLayer);

public:
	CLayer();
	~CLayer();

	friend class CLevel;
};

