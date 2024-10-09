#include "pch.h"
#include "CPlayerScript.h"
#include "CAnimStateMachine.h"
#include "CCameraMoveScript.h"

#include <Engine\CAnimator3D.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CTaskManager.h>

CPlayerScript::CPlayerScript()
	: CScript(SCRIPT_TYPE::PLAYERSCRIPT)
	, m_Speed(300.f)
	, m_RotSpeed(3.f)
	, m_IsDashing(false)
	, m_DashTime(0.f)
	, m_MaxDashTime(1.f)
	, m_InitialDashSpeed(50.f)
	, m_MaxDashSpeed(600.f)
{
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Speed", &m_Speed);
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Rot Speed", &m_RotSpeed);

	//m_vecFunc.push_back({ bind(&CPlayerScript::Move, this), "Move"});
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
	Ptr<CGraphicShader> pShader = GetOwner()->GetRenderComponent()->GetMaterial(0)->GetShader();
	pShader->AddScalarParam("Hit Event", INT_0);
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
			{
				m_ASM->ChangeState(AnimationState::RUN);
				Move(true);
			}
			else
			{
				m_ASM->ChangeState(AnimationState::MOVE);
				Move(false);
			}
		}
		else
		{
			m_ASM->ChangeState(AnimationState::IDLE);
		}
	}

	if (m_IsDashing)
	{
		DashForward();
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

void CPlayerScript::Move(bool isRun)
{
	CTransform* pTransform = GetOwner()->Transform();
	Vec3 originPos = pTransform->GetRelativePos();
	Vec3 playerDir = pTransform->GetRelativeDir(DIR_TYPE::RIGHT);
	
	if (KEY_PRESSED(KEY::A))
	{
		Vec3 curRot = pTransform->GetRelativeRotation();
		curRot.y -= DT_Engine * m_RotSpeed;
		pTransform->SetRelativeRotation(curRot);
	}
	else if (KEY_PRESSED(KEY::D))
	{
		Vec3 curRot = pTransform->GetRelativeRotation();
		curRot.y += DT_Engine * m_RotSpeed;
		pTransform->SetRelativeRotation(curRot);
	}

	if(!isRun)
		originPos += playerDir * DT_Engine * m_Speed;
	else
		originPos += playerDir * DT_Engine * m_Speed * 2.f;

	pTransform->SetRelativePos(originPos);
}

void CPlayerScript::AttackStart()
{
	m_ReadyAttack = false;
	m_IsAttacking = true;

	// ���� �� ���� ����
	m_IsDashing = true;
	m_DashTime = 0.0f; // ���� �ð� �ʱ�ȭ
	m_MaxDashTime = 0.5f; // 1�� ���� ����
	m_InitialDashSpeed = 50.f;
	m_MaxDashSpeed = 600.f; // ���� �ӵ� ����
}

void CPlayerScript::Attack()
{
	m_ReadyAttack = true;

	vector<CGameObject*> pChild = GetOwner()->GetChildren();
	CCameraMoveScript* pCameraScript = nullptr;

	for (size_t i = 0; i < pChild.size(); i++)
	{
		if (pChild[i]->GetScript<CCameraMoveScript>() != nullptr)
		{
			pCameraScript = pChild[i]->GetScript<CCameraMoveScript>();
			break;
		}
	}

	if (pCameraScript)
	{
		pCameraScript->StartCameraShake(10.0f, 0.05f); // ������ ���� �ð� ����
	}
}

void CPlayerScript::AttackEnd()
{
	m_ReadyAttack = false;
	m_IsAttacking = false;
	m_ASM->OnAnimationEnd();
}

void CPlayerScript::DashForward()
{
	if (m_DashTime < m_MaxDashTime)
	{
		// �ڻ��� �Լ� ��� ���ӵ� ����
		float progress = m_DashTime / m_MaxDashTime;  // ���� ���� (0.0f ~ 1.0f)
		float cosFactor = (1.0f - cosf(progress * XM_PI)) * 0.5f; // �ڻ������� õõ�� ���� �� �ް���

		// ���� �ӵ��� �ʱ� �ӵ��� �ִ� �ӵ� ������ ���� �ڻ��� �Լ��� ����
		float currentSpeed = m_InitialDashSpeed + cosFactor * (m_MaxDashSpeed - m_InitialDashSpeed);

		// �����ϴ� ����
		CTransform* pTransform = GetOwner()->Transform();
		Vec3 originPos = pTransform->GetRelativePos();
		Vec3 forwardDir = pTransform->GetRelativeDir(DIR_TYPE::RIGHT); // ���� ����

		// �ð��� ���� ��ȭ�ϴ� �ӵ��� ����
		originPos += forwardDir * currentSpeed * DT_Engine;
		pTransform->SetRelativePos(originPos);

		// ��� �ð� ������Ʈ
		m_DashTime += DT_Engine;
	}
	else
	{
		// ���� ����
		m_IsDashing = false;
	}
}

void CPlayerScript::Hit()
{
	m_IsHit = true;

	for (int i = 0; i < 4; i++)
	{
		GetOwner()->GetRenderComponent()->GetMaterial(i)->SetScalarParam(INT_0, 1);
	}
}

void CPlayerScript::HitEnd()
{
	m_IsHit = false;
	m_ASM->OnAnimationEnd();

	for (int i = 0; i < 4; i++)
	{
		GetOwner()->GetRenderComponent()->GetMaterial(i)->SetScalarParam(INT_0, 0);
	}
}

void CPlayerScript::Death()
{
	m_IsDeath = true;
}

void CPlayerScript::DeathEnd()
{
	GetOwner()->Destroy();
}