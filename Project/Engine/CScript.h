#pragma once
#include "CComponent.h"

#include "CTimeManager.h"
#include "CKeyManager.h"
#include "CAssetManager.h"

#include "components.h"

// �ܺο� �����Ű�� ���� ���� Type
enum class PROPERTY_TYPE
{
	INT,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	TEXTURE,
	GAMEOBJECT,
};

// �ܺο� �����Ű�� ���� ���� ����
struct tScriptProperty
{
	PROPERTY_TYPE	Type;		// ������ Type
	string			Desc;		// ������ ����
	void*			pData;		// ������ ���� (������)
};

// �߻� Ŭ���� ����
class CScript : public CComponent
{
private:
	const int					m_ScriptType;
	vector< tScriptProperty>	m_vecProperty;

protected:
	vector<std::pair<std::function<void()>, string>> m_vecFunc;

public:
	virtual void Tick() = 0; // Tick �� �� �����ϵ��� ����
	virtual void FinalTick() final {} // �� �����δ� FinalTick �� override �� �� ����

public:
	CGameObject* Instantiate(Ptr<CPrefab> prefab, int layerIdx, const Vec3& worldPos);
	UINT GetScriptType() { return m_ScriptType; }

	const vector<tScriptProperty>& GetScriptProperty() { return m_vecProperty; }
	vector<std::pair<std::function<void()>, string>>& GetVecFunc() { return m_vecFunc; }
	std::function<void()> GetEventCallbackByName(const std::string& eventName);

protected:
	void AddScriptProperty(PROPERTY_TYPE type, const string& desc, void* pData) 
	{
		m_vecProperty.push_back(tScriptProperty{ type, desc, pData });
	}

public:
	virtual void BeginOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) {}
	virtual void Overlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) {}
	virtual void EndOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) {}

public:
	virtual void SaveToLevelFile(FILE* file) override {};
	virtual void LoadFromLevelFile(FILE* file) override {};

public:
	CScript(UINT scriptType);
	~CScript();
};

