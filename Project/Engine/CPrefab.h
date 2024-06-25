#pragma once
#include "CAsset.h"

class CGameObject;

typedef void (*PREFAB_SAVE_FUNC)(CGameObject*, FILE*);
typedef CGameObject* (*PREFAB_LOAD_FUNC)(FILE*);

class CPrefab : public CAsset
{
	// Level Manager ���� �ϵ��ڵ����� Object �� ���� �� ������ �� ���� �ƴ϶�,
	// Editor �� ���� �ش� �۾��� �� �����̴�.

	// Unity �� Prefab ������ �ſ� �����ϴ�.

	// ���� ����δ� �ذ��� �Ұ��� (���� ����� ���� ��ü�� ������ �ؾ���. ����X )

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

