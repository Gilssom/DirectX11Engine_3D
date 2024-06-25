#include "pch.h"
#include "CMissileScript.h"

CMissileScript::CMissileScript()
	: CScript(SCRIPT_TYPE::MISSILESCRIPT)
	, m_Speed(500.f)
{

}

CMissileScript::~CMissileScript()
{

}

void CMissileScript::Begin()
{
	GetRenderComponent()->GetDynamicMaterial()->SetTexParam(TEX_0, CAssetManager::GetInst()->Load<CTexture>(L"texture\\Effect.png", L"texture\\Effect.png"));
}

void CMissileScript::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	vPos.y += m_Speed * DT;

	Transform()->SetRelativePos(vPos);
}