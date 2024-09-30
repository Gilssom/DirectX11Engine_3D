#include "pch.h"
#include "CPlayerScript.h"
#include "CAnimStateMachine.h"

#include <Engine\CAnimator3D.h>

CPlayerScript::CPlayerScript()
	: CScript(SCRIPT_TYPE::PLAYERSCRIPT)
{
	m_vecFunc.push_back({ bind(&CPlayerScript::Move, this), "Move"});
	m_vecFunc.push_back({ bind(&CPlayerScript::Attack, this), "Attack" });
	m_vecFunc.push_back({ bind(&CPlayerScript::AttackEnd, this), "Attack End" });
}

CPlayerScript::~CPlayerScript()
{
	m_vecFunc.clear();

	// �޸𸮱��� ��� ���� 
	// (clear �� ���� capacity �� �����ִ� �޸𸮱��� ó�� �Ϸ�)
	m_vecFunc.shrink_to_fit();
}

void CPlayerScript::Begin()
{

}

void CPlayerScript::Tick()
{
	//if (GetOwner()->GetScript<CAnimStateMachine>() && !m_ASM)
	//	m_ASM = GetOwner()->GetScript<CAnimStateMachine>();
}

void CPlayerScript::Move()
{
	int a = 0;
}

void CPlayerScript::Attack()
{
	if (GetOwner()->GetScript<CAnimStateMachine>() && !m_ASM)
		m_ASM = GetOwner()->GetScript<CAnimStateMachine>();

	m_ASM->ChangeState(AnimationState::ATTACK_0);
	GetOwner()->Animator3D()->PlayAnimation((UINT)m_ASM->GetCurState());
}

void CPlayerScript::AttackEnd()
{
	m_ASM->OnAnimationEnd();
	GetOwner()->Animator3D()->PlayAnimation((UINT)m_ASM->GetCurState());
}