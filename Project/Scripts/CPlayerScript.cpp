#include "pch.h"
#include "CPlayerScript.h"

#include <Engine\CAnimator3D.h>

CPlayerScript::CPlayerScript()
	: CScript(SCRIPT_TYPE::PLAYERSCRIPT)
{
	m_vecFunc.push_back({ bind(&CPlayerScript::Attack, this), "Attack"});
	m_vecFunc.push_back({ bind(&CPlayerScript::Move, this), "Move"});
}

CPlayerScript::~CPlayerScript()
{
	m_vecFunc.clear();
}

void CPlayerScript::Begin()
{

}

void CPlayerScript::Tick()
{

}

void CPlayerScript::Move()
{
	int a = 0;
}

void CPlayerScript::Attack()
{
	int a = 0;
}