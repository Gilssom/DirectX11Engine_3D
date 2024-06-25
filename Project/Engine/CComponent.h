#pragma once
#include "CEntity.h"

#include "CGameObject.h"

#define GET_OTHER_COMPONENT(Type) C##Type* Type() { return m_Owner->Type(); }

class CRenderComponent;

// 추상 클래스 정의
class CComponent : public CEntity
{
private:
	CGameObject*		 m_Owner; // 해당 컴포넌트를 소유하고 있는 오브젝트 주소
	const COMPONENT_TYPE m_Type;  // 본인이 무슨 컴포넌트인지에 대한 정보


public:
	virtual	void Begin() {};
	virtual	void Tick() {};
	virtual	void FinalTick() = 0; // 반드시 구현


public:
	COMPONENT_TYPE GetComponentType() { return m_Type; }
	CGameObject* GetOwner() { return m_Owner; }
	CRenderComponent* GetRenderComponent() { return m_Owner->GetRenderComponent(); }

	GET_OTHER_COMPONENT(Transform);
	GET_OTHER_COMPONENT(MeshRender);
	GET_OTHER_COMPONENT(Camera);
	GET_OTHER_COMPONENT(Collider2D);
	GET_OTHER_COMPONENT(Animator2D);
	GET_OTHER_COMPONENT(Light2D);
	GET_OTHER_COMPONENT(TileMap);
	GET_OTHER_COMPONENT(ParticleSystem);


	virtual void SaveToLevelFile(FILE* file) = 0;
	virtual void LoadFromLevelFile(FILE* file) = 0;

	// 반환 Type 재명시 (부모의 반환타입을 무조건 따라갈 필요 X)
	virtual CComponent* Clone() = 0;

public:
	CComponent(COMPONENT_TYPE type);
	CComponent(const CComponent& other);
	~CComponent();

	friend class CGameObject;
};

