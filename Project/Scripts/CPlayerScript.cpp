#include "pch.h"
#include "CPlayerScript.h"
#include "CAnimStateMachine.h"

#include <Engine\CAnimator3D.h>
#include <Engine\CTaskManager.h>

CPlayerScript::CPlayerScript()
	: CScript(SCRIPT_TYPE::PLAYERSCRIPT)
{
	m_vecFunc.push_back({ bind(&CPlayerScript::Move, this), "Move"});
	m_vecFunc.push_back({ bind(&CPlayerScript::Attack, this), "Attack" });
	m_vecFunc.push_back({ bind(&CPlayerScript::AttackEnd, this), "Attack End" });
	m_vecFunc.push_back({ bind(&CPlayerScript::AttackStart, this), "Attack Start" });
	m_vecFunc.push_back({ bind(&CPlayerScript::Hit, this), "Hit" });
	m_vecFunc.push_back({ bind(&CPlayerScript::HitEnd, this), "Hit End" });
	m_vecFunc.push_back({ bind(&CPlayerScript::Death, this), "Death" });
	m_vecFunc.push_back({ bind(&CPlayerScript::DeathEnd, this), "Death End" });
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
	if (GetOwner()->GetScript<CAnimStateMachine>() && !m_ASM)
		m_ASM = GetOwner()->GetScript<CAnimStateMachine>();

	if (GetOwner()->Animator3D()->GetEditorMode())
		return;

	if (m_IsDeath)
		return;

	if (!m_IsAttacking && !m_IsHit)
	{
		if (KEY_PRESSED(KEY::W))
		{
			if (KEY_PRESSED(KEY::LSHIFT))
				m_ASM->ChangeState(AnimationState::RUN);
			else
				m_ASM->ChangeState(AnimationState::MOVE);
		}
		else
		{
			m_ASM->ChangeState(AnimationState::IDLE);
		}
	}


	if (KEY_TAP(KEY::LBTN))
	{
		AnimationState curState = m_ASM->GetCurState();

		if (curState == AnimationState::IDLE || curState == AnimationState::MOVE || curState == AnimationState::RUN)
		{
			m_ASM->ChangeState(AnimationState::ATTACK_0);
		}
		else if (m_ReadyAttack && curState == AnimationState::ATTACK_0)
		{
			m_ASM->ChangeState(AnimationState::ATTACK_1);
		}
		else if (m_ReadyAttack && curState == AnimationState::ATTACK_1)
		{
			m_ASM->ChangeState(AnimationState::ATTACK_2);
		}
	}

	// Test
	if (KEY_TAP(KEY::X))
	{
		m_ASM->ChangeState(AnimationState::HIT);
	}

	if (KEY_TAP(KEY::C))
	{
		m_ASM->ChangeState(AnimationState::DEATH);
	}
}

void CPlayerScript::Move()
{
	
}

void CPlayerScript::AttackStart()
{
	m_ReadyAttack = false;
	m_IsAttacking = true;
}

void CPlayerScript::Attack()
{
	m_ReadyAttack = true;
}

void CPlayerScript::AttackEnd()
{
	m_ReadyAttack = false;
	m_IsAttacking = false;
	m_ASM->OnAnimationEnd();
}

void CPlayerScript::Hit()
{
	m_IsHit = true;
}

void CPlayerScript::HitEnd()
{
	m_IsHit = false;
	m_ASM->OnAnimationEnd();
}

void CPlayerScript::Death()
{
	m_IsDeath = true;
}

void CPlayerScript::DeathEnd()
{
	GetOwner()->Destroy();
}