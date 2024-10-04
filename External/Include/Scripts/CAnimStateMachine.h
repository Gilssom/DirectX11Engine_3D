#pragma once
#include <Engine\\CScript.h>

enum class AnimationState
{
	NONE,

	IDLE,
	MOVE,
	
	ATTACK_0,
	ATTACK_1,
	ATTACK_2,

	RUN,
	HIT,
	DEATH,

	END,
};

class CAnimStateMachine : public CScript
{
public:
	AnimationState m_CurState;
	AnimationState m_PrevState;
	std::stack<AnimationState> m_StateStack;

public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	void ChangeState(AnimationState newState);
	void ReturnPrevState();
	void OnAnimationEnd();

	AnimationState GetCurState() { return m_CurState; }

public:
	CLONE(CAnimStateMachine);

public:
	CAnimStateMachine();
	~CAnimStateMachine();
};