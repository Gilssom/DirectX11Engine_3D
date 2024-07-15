#pragma once
#include "CEntity.h"

#include "CGameObject.h"

#define GET_OTHER_COMPONENT(Type) C##Type* Type() { return m_Owner->Type(); }

class CRenderComponent;

// �߻� Ŭ���� ����
class CComponent : public CEntity
{
private:
	CGameObject*		 m_Owner; // �ش� ������Ʈ�� �����ϰ� �ִ� ������Ʈ �ּ�
	const COMPONENT_TYPE m_Type;  // ������ ���� ������Ʈ������ ���� ����


public:
	virtual	void Begin() {};
	virtual	void Tick() {};
	virtual	void FinalTick() = 0; // �ݵ�� ����


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
	GET_OTHER_COMPONENT(Light3D);
	GET_OTHER_COMPONENT(TileMap);
	GET_OTHER_COMPONENT(ParticleSystem);
	GET_OTHER_COMPONENT(SkyBox);
	GET_OTHER_COMPONENT(Decal);
	GET_OTHER_COMPONENT(BoundingBox);


	virtual void SaveToLevelFile(FILE* file) = 0;
	virtual void LoadFromLevelFile(FILE* file) = 0;

	// ��ȯ Type ���� (�θ��� ��ȯŸ���� ������ ���� �ʿ� X)
	virtual CComponent* Clone() = 0;

public:
	CComponent(COMPONENT_TYPE type);
	CComponent(const CComponent& other);
	~CComponent();

	friend class CGameObject;
};

