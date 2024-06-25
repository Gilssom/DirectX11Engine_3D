#include "pch.h"
#include "CGameObject.h"

#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"

#include "CComponent.h"
#include "CRenderComponent.h"
#include "CScript.h"

#include "CTaskManager.h"

CGameObject::CGameObject()
	: m_arrCom{}
	, m_RenderCom(nullptr)
	, m_Parent(nullptr)
	, m_LayerIdx(-1)
	, m_Dead(false)
{
}

// 복사 생성자를 구현할 때의 주의점은 부모의 복사 생성자도 명시적으로 지정해야 한다.
CGameObject::CGameObject(const CGameObject& other)
	: CEntity(other)
	, m_arrCom{} 
	, m_Parent(nullptr)
	, m_LayerIdx(-1)
	, m_Dead(false)
{
	// 원본 Object 의 Component 를 복제해서 받아옴 (복사X)
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (other.m_arrCom[i] != nullptr)
		{
			// Entity - Transform 의 중간 부모인 CComponent 에 Clone 가상 함수를 명시해줌.
			// 최종적으로 Clone 은 Transform 이나 각각의 Component 의 Clone 을 가져오게 됨.
			AddComponent(other.m_arrCom[i]->Clone());
		}
	}

	for (size_t i = 0; i < other.m_vecScripts.size(); i++)
	{
		AddComponent(other.m_vecScripts[i]->Clone());
	}

	for (size_t i = 0; i < other.m_vecChild.size(); i++)
	{
		// 재귀 (자식끼리의 복제하는 과정을 모두 거치고 오면 AddChild 가 이루어짐)
		AddChild(other.m_vecChild[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	Safe_Del_Array(m_arrCom);
	Safe_Del_Vector(m_vecScripts);
	Safe_Del_Vector(m_vecChild);
}

void CGameObject::Begin()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if(m_arrCom[i] != nullptr)
			m_arrCom[i]->Begin();
	}

	for (size_t i = 0; i < m_vecScripts.size(); i++)
	{
		m_vecScripts[i]->Begin();
	}

	// 자식 오브젝트 Begin 호출
	for (size_t i = 0; i < m_vecChild.size(); i++)
	{
		m_vecChild[i]->Begin();
	}
}

void CGameObject::Tick()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (m_arrCom[i] != nullptr)
			m_arrCom[i]->Tick();
	}

	for (size_t i = 0; i < m_vecScripts.size(); i++)
	{
		m_vecScripts[i]->Tick();
	}

	// 자식 오브젝트 Tick 호출
	for (size_t i = 0; i < m_vecChild.size(); i++)
	{
		m_vecChild[i]->Tick();
	}
}

void CGameObject::FinalTick()
{
	// Component Final Tick 호출
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (m_arrCom[i] != nullptr)
			m_arrCom[i]->FinalTick();
	}

	// 소속되어 있는 Layer에 자기 자신을 등록
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
	CLayer* pLayer = pCurLevel->GetLayer(m_LayerIdx);
	pLayer->RegisterObject(this);

	// 자식 오브젝트 Final Tick 호출
	for (size_t i = 0; i < m_vecChild.size(); i++)
	{
		m_vecChild[i]->FinalTick();
	}
}

void CGameObject::Render()
{
	if (m_RenderCom)
	{
		m_RenderCom->Render();
	}
}

void CGameObject::AddComponent(CComponent* component)
{
	// 입력으로 들어온 컴포넌트의 타입을 확인
	COMPONENT_TYPE type = component->GetComponentType();

	// 입력된 컴포넌트가 Script 종류인 경우
	if (COMPONENT_TYPE::SCRIPT == type)
	{
		m_vecScripts.push_back((CScript*)component);
	}
	else
	{
		// 컴포넌트 중복 방지
		assert(!m_arrCom[(UINT)type]);

		// 입력된 컴포넌트가 렌더링 관련 컴포넌트인지 확인 ( 다운 캐스팅으로 있는지 없는지 확인 )
		CRenderComponent* pRenderCom = dynamic_cast<CRenderComponent*>(component);

		if (pRenderCom != nullptr)
		{
			// 이미 렌더링 관련 컴포넌트를 보유하고 있다.
			assert(!m_RenderCom);

			m_RenderCom = pRenderCom;
		}

		// 입력된 컴포넌트를 배열의 알맞은 인덱스 자리에 주소값을 기록
		m_arrCom[(UINT)type] = component;
	}

	// 컴포넌트의 소유자를 자신으로 지정
	component->m_Owner = this;
}

void CGameObject::DisconnectWithParent()
{
	if (m_Parent == nullptr)
		return;

	// 부모와 자식 간의 계층 구조 해제
	vector<CGameObject*>::iterator iter = m_Parent->m_vecChild.begin();
	for (; iter != m_Parent->m_vecChild.end(); ++iter)
	{
		if ((*iter) == this)
		{
			m_Parent->m_vecChild.erase(iter);
			m_Parent = nullptr;
			return;
		}
	}

	// 자식 오브젝트는 부모를 지정했는데, 부모 오브젝트가 자식을 모르는 경우
	assert(nullptr);
}

void CGameObject::RegisterAsParentObject()
{
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
	CLayer* pCurLayer = pCurLevel->GetLayer(m_LayerIdx);
	pCurLayer->AddObject(this, false);
}

void CGameObject::AddChild(CGameObject* object)
{
	// 입력으로 들어오는 오브젝트가 이미 다른 부모의 자식인 경우 계층 구조 해제
	if (object->m_Parent)
	{
		object->DisconnectWithParent();
	}
	// 입력으로 들어오는 오브젝트가 최상위 부모 오브젝트이면서,
	// Level 내의 소속이 되어 있는 경우, Layer 부모 관리 기능에서 제외
	else if (object->m_LayerIdx != -1)
	{
		CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
		CLayer* pCurLayer = pCurLevel->GetLayer(object->m_LayerIdx);
		pCurLayer->DeRegisterParentObject(object);
	}

	// 받은 오브젝트를 "자식" 으로 설정 , 자식의 부모 오브젝트를 "자신" 으로 설정
	object->m_Parent = this;
	m_vecChild.push_back(object);
}

bool CGameObject::IsAncestor(CGameObject* object)
{
	CGameObject* pAncestor = m_Parent;

	while (pAncestor)
	{
		if (pAncestor == object)
			return true;

		pAncestor = pAncestor->GetParent();
	}

	return false;
}

void CGameObject::Destroy()
{
	tTask task = {};

	task.Type = TASK_TYPE::DESTROY_OBJECT;
	task.dwParam_0 = (DWORD_PTR)this;

	CTaskManager::GetInst()->AddTask(task);
}