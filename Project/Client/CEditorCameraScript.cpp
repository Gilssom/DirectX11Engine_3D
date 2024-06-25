#include "pch.h"
#include "CEditorCameraScript.h"

#include <Engine\\CKeyManager.h>

CEditorCameraScript::CEditorCameraScript()
	: CScript(-1)
	, m_Speed(200.f)
{

}

CEditorCameraScript::~CEditorCameraScript()
{

}

void CEditorCameraScript::Tick()
{
	// Editor 의 Tick 은 별도로 Tick 을 돌려주기 때문에
	// Level 이 정지되었든 상관이없다.

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

	if (Camera()->GetProjType() == PROJ_TYPE::PERSPECTIVE)
		MoveByPerspective();

	else
		MoveByOrthographic();
}

void CEditorCameraScript::MoveByPerspective()
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
		vCurPos += vFront * speed * DT_Engine;
	}
	if (KEY_PRESSED(KEY::DOWN))
	{
		vCurPos += -vFront * speed * DT_Engine;
	}
	if (KEY_PRESSED(KEY::LEFT))
	{
		vCurPos += -vRight * speed * DT_Engine;
	}
	if (KEY_PRESSED(KEY::RIGHT))
	{
		vCurPos += vRight * speed * DT_Engine;
	}

	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vDrag = CKeyManager::GetInst()->GetMouseDrag();

		Vec3 vRot = Transform()->GetRelativeRotation();
		vRot.y += vDrag.x * DT_Engine * XM_PI * 10.f;
		vRot.x -= vDrag.y * DT_Engine * XM_PI * 10.f;

		Transform()->SetRelativeRotation(vRot);
	}

	GetOwner()->Transform()->SetRelativePos(vCurPos);
}

void CEditorCameraScript::MoveByOrthographic()
{
	// Shift 속도 배율
	float speed = m_Speed;
	if (KEY_PRESSED(KEY::LSHIFT))
	{
		speed *= 4.f;
	}

	// 키 입력에 따른 위치 이동
	Vec3 vCurPos = GetOwner()->Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::UP))
	{
		vCurPos.y += speed * DT_Engine;
	}
	if (KEY_PRESSED(KEY::DOWN))
	{
		vCurPos.y -= speed * DT_Engine;
	}
	if (KEY_PRESSED(KEY::LEFT))
	{
		vCurPos.x -= speed * DT_Engine;
	}
	if (KEY_PRESSED(KEY::RIGHT))
	{
		vCurPos.x += speed * DT_Engine;
	}

	if (KEY_PRESSED(KEY::NUM1))
	{
		float scale = Camera()->GetScale();
		scale -= DT_Engine;

		if (scale < 0.01f)
		{
			scale = 0.01f;
		}

		Camera()->SetScale(scale);
	}

	if (KEY_PRESSED(KEY::NUM2))
	{
		float scale = Camera()->GetScale();
		scale += DT_Engine;
		Camera()->SetScale(scale);
	}

	GetOwner()->Transform()->SetRelativePos(vCurPos);
}