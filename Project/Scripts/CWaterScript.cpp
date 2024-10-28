#include "pch.h"
#include "CWaterScript.h"

#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>

CWaterScript::CWaterScript()
	: CScript(SCRIPT_TYPE::WATERSCRIPT)
	, m_WaterNormalMap(nullptr)
	, m_CamPos(0.f)
	, m_FresnelPower(5.f)
	, m_NormalStrength(5.f)
{
	//AddScriptProperty(PROPERTY_TYPE::TEXTURE, "Water Normal", &m_WaterNormalMap);
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Fresnel Power", &m_FresnelPower);
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Normal Strength", &m_NormalStrength);
}

CWaterScript::~CWaterScript()
{

}

void CWaterScript::Begin()
{

}

void CWaterScript::Tick()
{
	Ptr<CMaterial> pMaterial = GetOwner()->GetRenderComponent()->GetMaterial(0);
	//pMaterial->SetTexParam(TEX_1, m_WaterNormalMap);
	pMaterial->SetScalarParam(FLOAT_0, m_FresnelPower);
	pMaterial->SetScalarParam(FLOAT_1, m_NormalStrength);

	CGameObject* pMainCam = CLevelManager::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	pMaterial->SetScalarParam(VEC4_0, pMainCam->Transform()->GetRelativePos());
}