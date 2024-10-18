#include "pch.h"
#include "CPlayerScript.h"
#include "CAnimStateMachine.h"
#include "CCameraMoveScript.h"

#include <Engine\CAnimator3D.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CTaskManager.h>
#include <Engine\CParticleSystem.h>

CPlayerScript::CPlayerScript()
	: CScript(SCRIPT_TYPE::PLAYERSCRIPT)
	, m_Speed(300.f)
	, m_RotSpeed(3.f)
	, m_IsDashing(false)
	, m_DashTime(0.f)
	, m_MaxDashTime(1.f)
	, m_InitialDashSpeed(50.f)
	, m_MaxDashSpeed(600.f)
	, m_WeaponIsEmissive(false)
	, m_WeaponEmissive(0.f)
{
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Speed", &m_Speed);
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Rot Speed", &m_RotSpeed);
	AddScriptProperty(PROPERTY_TYPE::INT, "Weapon Is Emissive", &m_WeaponIsEmissive);
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Weapon Emissive", &m_WeaponEmissive);

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

	// 메모리까지 모두 해제 
	// (clear 로 인한 capacity 가 남아있던 메모리까지 처리 완료)
	m_vecFunc.shrink_to_fit();
}

void CPlayerScript::Begin()
{
	Ptr<CGraphicShader> pShader = GetOwner()->GetRenderComponent()->GetMaterial(0)->GetShader();
	pShader->AddScalarParam("Hit Event", INT_0);

	pShader = GetOwner()->GetRenderComponent()->GetMaterial(3)->GetShader();
	pShader->AddScalarParam("Trail On Off", INT_2);
	pShader->AddScalarParam("Trail Alpha", FLOAT_1);
	pShader->AddScalarParam("Trail Length", FLOAT_2);
	
	// 최대 4개의 검 궤적 위치만 바인딩 가능
	pShader->AddScalarParam("SwordPosition0", VEC4_0);
	pShader->AddScalarParam("SwordPosition1", VEC4_1);
	pShader->AddScalarParam("SwordPosition2", VEC4_2);
	pShader->AddScalarParam("SwordPosition3", VEC4_3);

	//m_Test = CAssetManager::GetInst()->FindAsset<CPrefab>(L"Prefab\\HitEffect.pref");
	m_Test = CAssetManager::GetInst()->Load<CPrefab>(L"Prefab\\HitEffect.pref", L"Prefab\\HitEffect.pref");
}

void CPlayerScript::Tick()
{
	Ptr<CMaterial> pMaterial = GetOwner()->GetRenderComponent()->GetMaterial(3);
	pMaterial->SetScalarParam(INT_1, m_WeaponIsEmissive);
	pMaterial->SetScalarParam(FLOAT_0, m_WeaponEmissive);

	Vec3 vSwordLocalPos = GetOwner()->Animator3D()->GetObjectPosition(L"Dummy");
	Vec3 vSwordLocalRot = GetOwner()->Animator3D()->GetObjectRotation(L"Dummy");

	CGameObject* pEffect = GetOwner()->FindChildrenByName(L"Slash Effect");
	pEffect->Transform()->SetRelativePos(Vec3(vSwordLocalPos.x, vSwordLocalPos.y, vSwordLocalPos.z));
	pEffect->Transform()->SetRelativeRotation(vSwordLocalRot);

	Vec3 vPlayerWorldPos = GetOwner()->Transform()->GetWorldPos();
	Vec3 vPlayerWorldRot = GetOwner()->Transform()->GetWorldRotation();
	Vec3 vFinalSwordPos = vPlayerWorldPos + vSwordLocalPos;
	Vec3 vFinalSwordRot = vPlayerWorldRot * vSwordLocalRot; // 월드 회전 계산 (쿼터니언 곱셈)

	UpdateSwordTrail(vFinalSwordPos, vFinalSwordRot);
	
	// 검의 궤적 4개만 Shader에 전달
	if (m_SwordTrail.size() > 0)
		pMaterial->SetScalarParam(VEC4_0, Vec4(m_SwordTrail[0].position, 1.0f));
	if (m_SwordTrail.size() > 1)
		pMaterial->SetScalarParam(VEC4_1, Vec4(m_SwordTrail[1].position, 1.0f));
	if (m_SwordTrail.size() > 2)
		pMaterial->SetScalarParam(VEC4_2, Vec4(m_SwordTrail[2].position, 1.0f));
	if (m_SwordTrail.size() > 3)
		pMaterial->SetScalarParam(VEC4_3, Vec4(m_SwordTrail[3].position, 1.0f));


	// 남은 궤적은 0으로 설정 (이전 Frame의 값을 지우기 위함)
	for (int i = m_SwordTrail.size(); i < 4; ++i)
	{
		std::string paramName = "SwordPosition" + std::to_string(i);
		pMaterial->SetScalarParam(static_cast<SCALAR_PARAM>(VEC4_0 + i), Vec4(0.f, 0.f, 0.f, 0.f));
	}

	if (m_WeaponEmissive >= 1.f)
		m_WeaponEmissive = 1.f;

	if (m_WeaponEmissive < 0.f)
		m_WeaponEmissive = 0.f;

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

	if (KEY_TAP(KEY::F))
	{
		//return;

		m_WeaponIsEmissive = true;
		m_WeaponEmissive = 0.8f;

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

	// 공격 시 돌진 시작
	m_IsDashing = true;
	m_DashTime = 0.0f; // 돌진 시간 초기화
	m_MaxDashTime = 0.5f; // 1초 동안 돌진
	m_InitialDashSpeed = 50.f;
	m_MaxDashSpeed = 600.f; // 돌진 속도 설정
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
		pCameraScript->StartCameraShake(10.0f, 0.05f); // 강도와 지속 시간 설정
	}

	//SlashEffect();
}

void CPlayerScript::AttackEnd()
{
	m_ReadyAttack = false;
	m_IsAttacking = false;
	m_ASM->OnAnimationEnd();

	m_WeaponIsEmissive = false;
	m_WeaponEmissive = 0.f;
}

void CPlayerScript::SlashEffect()
{
	// 슬래시 파티클 이펙트 생성 및 설정
	CGameObject* pSlash = new CGameObject;
	pSlash->SetName(L"Slash Effect");
	pSlash->AddComponent(new CTransform);
	pSlash->AddComponent(new CParticleSystem);
	pSlash->Transform()->SetRelativePos(Transform()->GetRelativePos());
	CParticleSystem* pSlashEffect = pSlash->ParticleSystem();

	CLevelManager::GetInst()->GetCurrentLevel()->AddObject(0, pSlash, true);

	// 파티클 텍스처 설정 (슬래시 모양 텍스처)
	Ptr<CTexture> pSlashTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\Effect.png");
	pSlashEffect->SetParticleTexture(pSlashTex);

	// 파티클 속성 설정
	pSlashEffect->SetMaxParticleCount(1000); // 파티클 개수 설정

	// 파티클 모듈 설정 (슬래시 모양으로 확산되는 효과)
	pSlashEffect->SetSpawnRate(500); // 초당 생성될 파티클 개수
	pSlashEffect->SetLife(0.1f, 0.3f); // 파티클 최소, 최대 수명
	pSlashEffect->SetScale(Vec3(20.f, 5.f, 1.f), Vec3(50.f, 10.f, 1.f)); // 크기 설정
	pSlashEffect->SetVelocity(Vec3(1.f, 0.f, 0.f), 500.f, 700.f); // 속도 설정
	pSlashEffect->SetColor(Vec4(1.f, 0.f, 0.f, 1.f), Vec3(1.f, 1.f, 1.f)); // 색상 설정
	pSlashEffect->SetFadeInOut(true, 0.5f);
	pSlashEffect->SetModuleOnOff(PARTICLE_MODULE::SPAWN_BURST, false);
}

void CPlayerScript::DashForward()
{
	if (m_DashTime < m_MaxDashTime)
	{
		// 코사인 함수 기반 가속도 적용
		float progress = m_DashTime / m_MaxDashTime;  // 진행 비율 (0.0f ~ 1.0f)
		float cosFactor = (1.0f - cosf(progress * XM_PI)) * 0.5f; // 코사인으로 천천히 가속 후 급가속

		// 현재 속도는 초기 속도와 최대 속도 사이의 값을 코사인 함수로 조절
		float currentSpeed = m_InitialDashSpeed + cosFactor * (m_MaxDashSpeed - m_InitialDashSpeed);

		// 전진하는 로직
		CTransform* pTransform = GetOwner()->Transform();
		Vec3 originPos = pTransform->GetRelativePos();
		Vec3 forwardDir = pTransform->GetRelativeDir(DIR_TYPE::RIGHT); // 전방 방향

		// 시간에 따라 변화하는 속도로 전진
		originPos += forwardDir * currentSpeed * DT_Engine;
		pTransform->SetRelativePos(originPos);

		// 경과 시간 업데이트
		m_DashTime += DT_Engine;
	}
	else
	{
		// 돌진 종료
		m_IsDashing = false;
	}
}

void CPlayerScript::Hit()
{
	if (!m_IsHit)
	{
		Vec3 spawnPos = Vec3(Transform()->GetRelativePos().x, Transform()->GetRelativePos().y + 300.f, Transform()->GetRelativePos().z);
		CGameObject* pParticle = Instantiate(m_Test, 0, spawnPos);
		pParticle->SetName(L"Effect");
		pParticle->GetRenderComponent()->SetFrustumCheck(true);
		pParticle->ParticleSystem()->UpdateModuleBuffer();
		pParticle->ParticleSystem()->SetIsPlayed(true);
	}
	
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

void CPlayerScript::UpdateSwordTrail(const Vec3& swordPos, const Vec3& swordRot)
{
	// 일정 시간(0.3초)마다만 업데이트 되도록 함
	static float m_LastUpdateTime = 0.0f;

	// 오래된 Trail 제거 (예: 1초 이상 지난 것)
	for (auto& segment : m_SwordTrail)
	{
		segment.timeAlive += DT;
	}

	// 오래된 Trail 제거
	while (!m_SwordTrail.empty() && m_SwordTrail.front().timeAlive > 5.0f)
	{
		m_SwordTrail.pop_front();
	}

	// 0.3초가 경과하지 않았다면 리턴
	if (m_LastUpdateTime < 0.5f)
	{
		m_LastUpdateTime += DT;
		return;
	}

	// 궤적을 업데이트했으므로 시간을 초기화
	m_LastUpdateTime = 0.0f;

	// 새로운 위치를 deque에 추가
	TrailSegment newSegment;
	newSegment.position = swordPos;
	newSegment.rotation = swordRot;
	newSegment.timeAlive = 0.0f;

	m_SwordTrail.push_back(newSegment);
}