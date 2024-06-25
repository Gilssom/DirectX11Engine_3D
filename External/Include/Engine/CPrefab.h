#pragma once
#include "CAsset.h"

class CGameObject;

typedef void (*PREFAB_SAVE_FUNC)(CGameObject*, FILE*);
typedef CGameObject* (*PREFAB_LOAD_FUNC)(FILE*);

class CPrefab : public CAsset
{
	// Level Manager 에서 하드코딩으로 Object 를 생성 및 설정을 할 것이 아니라,
	// Editor 를 통해 해당 작업을 할 예정이다.

	// Unity 의 Prefab 구도와 매우 유사하다.

	// 얕은 복사로는 해결이 불가능 (복사 대상의 정보 전체를 복제를 해야함. 복사X )

private:
	static PREFAB_SAVE_FUNC SAVE_FUNC;
	static PREFAB_LOAD_FUNC LOAD_FUNC;

public:
	static void SetPrefabSaveFunc(PREFAB_SAVE_FUNC func) { SAVE_FUNC = func; }
	static void SetPrefabLoadFunc(PREFAB_LOAD_FUNC func) { LOAD_FUNC = func; }

private:
	CGameObject* m_ProtoObject;

public:
	CGameObject* Instantiate();

public:
	virtual int Save(const wstring& FilePath) override;
	virtual int Load(const wstring& FilePath) override;

	CLONE(CPrefab);

public:
	CPrefab(bool bEngine = false);
	CPrefab(CGameObject* proto);
	CPrefab(const CPrefab& other);
	~CPrefab();

};

