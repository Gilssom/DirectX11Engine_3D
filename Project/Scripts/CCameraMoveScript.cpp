#include "pch.h"
#include "CCameraMoveScript.h"

#include <Engine\\CCamera.h>
#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>

#include <Engine\\CLevelManager.h>
#include <Engine\\CLevel.h>
				 
#include <Engine\\CDevice.h>

CCameraMoveScript::CCameraMoveScript()
	: CScript(SCRIPT_TYPE::CAMERAMOVESCRIPT)
	, m_TargetObject(nullptr)
	, m_Speed(200.f)
{
	AddScriptProperty(PROPERTY_TYPE::GAMEOBJECT, "Target Object", &m_TargetObject);
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Speed", &m_Speed);
}

CCameraMoveScript::~CCameraMoveScript()
{

}

void CCameraMoveScript::Begin()
{
	Transform()->SetRelativeRotation(25.519f, 89.465f, 0.f);
}

void CCameraMoveScript::Tick()
{
	//Transform()->SetRelativeRotation(24.7f, 87.5f, 0.f);

	// 카메라 투영 모드 변경
	if (KEY_TAP(KEY::P))
	{
		Camera()->GetProjType() == PROJ_TYPE::ORTHOGRAPHIC ? Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE) : Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);

		if (Camera()->GetProjType() == PROJ_TYPE::ORTHOGRAPHIC)
		{
			Vec3 vPos = Transform()->GetRelativePos();
			Transform()->SetRelativePos(Vec3(vPos.x, vPos.y, 0.f));
			Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
		}
	}

	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
	if(pCurLevel != nullptr)
		m_TargetObject = pCurLevel->FindObjectByName(L"Player");

	if (m_IsShaking)
	{
		UpdateCameraShake();
	}

	if (Camera()->GetProjType() == PROJ_TYPE::PERSPECTIVE)
		MoveByPerspective();

	else
		MoveByOrthographic();
}

void CCameraMoveScript::StartCameraShake(float intensity, float duration)
{
	m_IsShaking = true;
	m_ShakeIntensity = intensity;
	m_ShakeDuration = duration;
	m_ShakeTime = 0.0f;
	m_OriginalPos = GetOwner()->Transform()->GetRelativePos(); // 카메라 원래 위치 저장
}

void CCameraMoveScript::UpdateCameraShake()
{
	if (m_ShakeTime < m_ShakeDuration)
	{
		// 랜덤한 오프셋을 계산하여 카메라를 흔듬
		float offsetX = ((float)rand() / RAND_MAX - 0.5f) * m_ShakeIntensity;
		float offsetY = ((float)rand() / RAND_MAX - 0.5f) * m_ShakeIntensity;
		float offsetZ = ((float)rand() / RAND_MAX - 0.5f) * m_ShakeIntensity;

		Vec3 shakePos = m_OriginalPos + Vec3(offsetX, offsetY, offsetZ);
		GetOwner()->Transform()->SetRelativePos(shakePos);

		m_ShakeTime += DT; // 경과 시간 업데이트
	}
	else
	{
		// 쉐이크가 끝나면 원래 위치로 복원
		m_IsShaking = false;
		GetOwner()->Transform()->SetRelativePos(m_OriginalPos);
	}
}

void CCameraMoveScript::MoveByPerspective()
{
	// Shift 속도 배율
	float speed = m_Speed;
	if (KEY_PRESSED(KEY::LSHIFT))
	{
		speed *= 4.f;
	}

	// 키 입력에 따른 위치 이동
	Vec3 vCurPos = GetOwner()->Transform()->GetRelativePos();

	Vec3 vFront = Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vRight = Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	if (KEY_PRESSED(KEY::UP))
	{
		vCurPos += vFront * speed * DT;
	}
	if (KEY_PRESSED(KEY::DOWN))
	{
		vCurPos += -vFront * speed * DT;
	}
	if (KEY_PRESSED(KEY::LEFT))
	{
		vCurPos += -vRight * speed * DT;
	}
	if (KEY_PRESSED(KEY::RIGHT))
	{
		vCurPos += vRight * speed * DT;
	}

	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vDrag = CKeyManager::GetInst()->GetMouseDrag();

		Vec3 vRot = Transform()->GetRelativeRotation();
		vRot.y += vDrag.x * DT * XM_PI * 10.f;
		vRot.x -= vDrag.y * DT * XM_PI * 10.f;

		Transform()->SetRelativeRotation(vRot);
	}

	GetOwner()->Transform()->SetRelativePos(vCurPos);
}

void CCameraMoveScript::MoveByOrthographic()
{
	// 카메라 위치 최종 세팅
}