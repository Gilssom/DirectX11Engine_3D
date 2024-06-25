#pragma once
#include "CComponent.h"

#include "CTimeManager.h"
#include "CKeyManager.h"
#include "CAssetManager.h"

#include "components.h"

// 외부에 노출시키고 싶은 변수 Type
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

// 외부에 노출시키고 싶은 변수 설정
struct tScriptProperty
{
	PROPERTY_TYPE	Type;		// 변수의 Type
	string			Desc;		// 변수의 설명
	void*			pData;		// 변수의 원본 (포인터)
};

// 추상 클래스 정의
class CScript : public CComponent
{
private:
	const int					m_ScriptType;
	vector< tScriptProperty>	m_vecProperty;

public:
	virtual void Tick() = 0; // Tick 은 꼭 구현하도록 강제
	virtual void FinalTick() final {} // 이 밑으로는 FinalTick 을 override 할 수 없게

public:
	CGameObject* Instantiate(Ptr<CPrefab> prefab, int layerIdx, const Vec3& worldPos);
	UINT GetScriptType() { return m_ScriptType; }

	const vector<tScriptProperty>& GetScriptProperty() { return m_vecProperty; }

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

