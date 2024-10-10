#include "pch.h"
#include "CAnimStateMachine.h"

CAnimStateMachine::CAnimStateMachine()
	: CScript(ANIMSTATEMACHINE)
	, m_CurState(AnimationState::IDLE)
	, m_PrevState(AnimationState::IDLE)
{

}

CAnimStateMachine::~CAnimStateMachine()
{

}

void CAnimStateMachine::Begin()
{
	m_PrevState = m_CurState;
	m_CurState = AnimationState::IDLE;
	m_StateStack.push(m_CurState);

	GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::IDLE);
}

void CAnimStateMachine::Tick()
{

}

void CAnimStateMachine::ChangeState(AnimationState newState)
{
	if (m_PrevState == newState)
		return;

	m_PrevState = m_CurState;
	m_CurState = newState;
	m_StateStack.push(newState);

	switch (m_CurState)
	{
	case AnimationState::IDLE:
		GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::IDLE);
		break;
	case AnimationState::MOVE:
		GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::MOVE);
		break;
	case AnimationState::ATTACK_0:
		GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::ATTACK_0);
		break;
	case AnimationState::ATTACK_1:
		GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::ATTACK_1);
		break;
	case AnimationState::ATTACK_2:
		GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::ATTACK_2);
		break;
	case AnimationState::RUN:
		GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::RUN);
		break;
	case AnimationState::HIT:
		GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::HIT);
		break;
	case AnimationState::DEATH:
		GetOwner()->Animator3D()->PlayAnimation((UINT)AnimationState::DEATH);
		break;
	}
}

void CAnimStateMachine::ReturnPrevState()
{
	if (!m_StateStack.empty())
	{
		m_StateStack.pop();
		m_CurState = !m_StateStack.empty() ? m_StateStack.top() : m_PrevState;
		ChangeState(m_CurState);
	}
}

void CAnimStateMachine::OnAnimationEnd()
{
	ReturnPrevState();
}