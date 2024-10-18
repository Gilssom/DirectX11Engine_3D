#pragma once
#include "CEntity.h"

class CComponent;
class CRenderComponent;
class CScript;

#define GET_COMPONENT(Type, TYPE) class C##Type* Type() { return (C##Type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE]; }

class CGameObject : public CEntity
{
private:
	CComponent*				m_arrCom[(UINT)COMPONENT_TYPE::END];
	CRenderComponent*		m_RenderCom;
	vector<CScript*>		m_vecScripts;

	// ������Ʈ�� ���� ���� ���� �ʿ�
	CGameObject*			m_Parent;		// �θ� ������Ʈ ������
	vector<CGameObject*>	m_vecChild;		// �ڽ� ������Ʈ�� ������ vector

	int						m_LayerIdx;		// GameObject �� �ҼӵǾ� �ִ� Layer
	bool					m_Dead;			// ������Ʈ Life üũ


public:
	void Begin();				// ������ ���� �� ��, ������ �շ��� ��
	void Tick();				// �� �����Ӹ��� ȣ��Ǵ� �Լ� Delta Time
	virtual void FinalTick();	// Tick ���� �߻��� �ϵ��� ������ �۾� or ���ҽ� ���ε� �� ���� ������ ����
	void Render();				// ȭ�鿡 �׷����� �Լ�


public:
	void AddComponent(CComponent* component);
	CComponent* GetComponent(COMPONENT_TYPE type) { return m_arrCom[(UINT)type]; }
	CRenderComponent* GetRenderComponent() { return m_RenderCom; }

	GET_COMPONENT(Transform, TRANSFORM);
	GET_COMPONENT(MeshRender, MESHRENDER);
	GET_COMPONENT(Camera, CAMERA);
	GET_COMPONENT(Collider2D, COLLIDER2D);
	GET_COMPONENT(Animator2D, ANIMATOR2D);
	GET_COMPONENT(Animator3D, ANIMATOR3D);
	GET_COMPONENT(Light2D, LIGHT2D);
	GET_COMPONENT(Light3D, LIGHT3D);
	GET_COMPONENT(TileMap, TILEMAP);
	GET_COMPONENT(ParticleSystem, PARTICLESYSTEM);
	GET_COMPONENT(SkyBox, SKYBOX);
	GET_COMPONENT(Decal, DECAL);
	GET_COMPONENT(BoundingBox, BOUNDINGBOX);
	GET_COMPONENT(LandScape, LANDSCAPE);

	void DisconnectWithParent();
	void RegisterAsParentObject();
	void AddChild(CGameObject* object);
	const vector<CGameObject*>& GetChildren() { return m_vecChild; }
	CGameObject* FindChildrenByName(const wstring& findName);
	CGameObject* GetParent() { return m_Parent; }
	const vector<CScript*>& GetScripts() { return m_vecScripts; }
	bool IsAncestor(CGameObject* object);

	template<typename T>
	T* GetScript();

	bool IsDead() { return m_Dead; }
	void Destroy();

	int GetLayerIdx() { return m_LayerIdx; }
	void ChangeLayerIdx(int changeLayer) { m_LayerIdx = changeLayer; }

private:
	void SetLayerIdx(int idx) { m_LayerIdx = idx; }

public:
	CLONE(CGameObject);

public:
	CGameObject();

	// GameObject �� ���� ����� �ȵǰ� ���� �����ڸ� ���� �����ؾ��Ѵ�. 
	// Component �̳� ��� ������ �� ����(����X) �� �Ǿ�� �ϱ� ����
	CGameObject(const CGameObject& other);

	~CGameObject();

	// Layer ������ Layer Index �� �ǵ��� �� �ְ�
	friend class CLayer;

	// Task Manager ������ Dead �� �ǵ��� �� �ְ�
	friend class CTaskManager;
};

template<typename T>
inline T* CGameObject::GetScript()
{
	for (size_t i = 0; i < m_vecScripts.size(); i++)
	{
		if (dynamic_cast<T*>(m_vecScripts[i]))
			return (T*)m_vecScripts[i];
	}

	return nullptr;
}
