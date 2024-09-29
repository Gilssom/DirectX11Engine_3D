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

}

void CAnimStateMachine::Tick()
{

}

void CAnimStateMachine::ChangeState(AnimationState newState)
{
	m_PrevState = m_CurState;
	m_CurState = newState;
	m_StateStack.push(newState);
}

void CAnimStateMachine::ReturnPrevState()
{
	if (!m_StateStack.empty())
	{
		m_StateStack.pop();
		m_CurState = !m_StateStack.empty() ? m_StateStack.top() : m_PrevState;
	}
}

void CAnimStateMachine::OnAnimationEnd()
{
	ReturnPrevState();
}