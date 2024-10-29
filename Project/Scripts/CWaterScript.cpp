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
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Wave Speed", &m_WaveSpeed);
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Wave Amplitude", &m_WaveAmplitude);
	AddScriptProperty(PROPERTY_TYPE::FLOAT, "Wave Frequency", &m_WaveFrequency);
}

CWaterScript::~CWaterScript()
{

}

void CWaterScript::Begin()
{
	GetOwner()->GetRenderComponent()->SetDynamicShadow(false);
}

void CWaterScript::Tick()
{
	Ptr<CMaterial> pMaterial = GetOwner()->GetRenderComponent()->GetMaterial(0);
	//pMaterial->SetTexParam(TEX_1, m_WaterNormalMap);
	pMaterial->SetScalarParam(FLOAT_0, m_WaveSpeed);
	pMaterial->SetScalarParam(FLOAT_1, m_WaveAmplitude);
	pMaterial->SetScalarParam(FLOAT_2, m_WaveFrequency);

	CGameObject* pMainCam = CLevelManager::GetInst()->GetCurrentLevel()->FindObjectByName(L"MainCamera");
	pMaterial->SetScalarParam(VEC4_0, pMainCam->Transform()->GetRelativePos());

	Vec3 vTest = GetOwner()->Transform()->GetRelativeScale();
	pMaterial->SetScalarParam(VEC2_0, pMainCam->Transform()->GetRelativeScale());
}