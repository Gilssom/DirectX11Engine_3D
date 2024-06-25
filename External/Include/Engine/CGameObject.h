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

	// 오브젝트의 계층 구조 관계 필요
	CGameObject*			m_Parent;		// 부모 오브젝트 포인터
	vector<CGameObject*>	m_vecChild;		// 자식 오브젝트들 포인터 vector

	int						m_LayerIdx;		// GameObject 가 소속되어 있는 Layer
	bool					m_Dead;			// 오브젝트 Life 체크


public:
	void Begin();				// 레벨이 시작 될 때, 레벨에 합류할 때
	void Tick();				// 매 프레임마다 호출되는 함수 Delta Time
	virtual void FinalTick();	// Tick 에서 발생한 일들의 마무리 작업 or 리소스 바인딩 및 관련 데이터 연산
	void Render();				// 화면에 그려지는 함수


public:
	void AddComponent(CComponent* component);
	CComponent* GetComponent(COMPONENT_TYPE type) { return m_arrCom[(UINT)type]; }
	CRenderComponent* GetRenderComponent() { return m_RenderCom; }

	GET_COMPONENT(Transform, TRANSFORM);
	GET_COMPONENT(MeshRender, MESHRENDER);
	GET_COMPONENT(Camera, CAMERA);
	GET_COMPONENT(Collider2D, COLLIDER2D);
	GET_COMPONENT(Animator2D, ANIMATOR2D);
	GET_COMPONENT(Light2D, LIGHT2D);
	GET_COMPONENT(TileMap, TILEMAP);
	GET_COMPONENT(ParticleSystem, PARTICLESYSTEM);

	void DisconnectWithParent();
	void RegisterAsParentObject();
	void AddChild(CGameObject* object);
	const vector<CGameObject*>& GetChildren() { return m_vecChild; }
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

	// GameObject 는 얕은 복사는 안되고 복사 생성자를 직접 구현해야한다. 
	// Component 이나 모든 정보가 다 복제(복사X) 가 되어야 하기 때문
	CGameObject(const CGameObject& other);

	~CGameObject();

	// Layer 에서만 Layer Index 를 건들일 수 있게
	friend class CLayer;

	// Task Manager 에서만 Dead 를 건들일 수 있게
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
