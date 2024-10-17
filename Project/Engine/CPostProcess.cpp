#include "pch.h"
//#include "CPostProcess.h"
//
//#include "CGameObject.h"
//#include "CMesh.h"
//#include "CMeshRender.h"
//#include "CMaterial.h"
//
//CPostProcess::CPostProcess()
//	: CComponent(COMPONENT_TYPE::POSTPROCESS)
//	, m_BloomPower(0.93f)
//	, m_BlurRadius(4.45f)
//	, m_BrightnessScale(1.0f)
//	, m_BloomThreshold(1.25f)
//	, m_Radius(0.87f)
//	, m_Sofeness(0.73f)
//	, m_FocusDistance(0.6f)
//	, m_FocusRange(0.25f)
//{
//	m_PostMaterial.push_back(CAssetManager::GetInst()->FindAsset<CMaterial>(L"BloomMaterial"));
//	m_PostMaterial.push_back(CAssetManager::GetInst()->FindAsset<CMaterial>(L"VignetteMaterial"));
//	m_PostMaterial.push_back(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DepthOfFieldMaterial"));
//}
//
//CPostProcess::CPostProcess(const CPostProcess& origin)
//	: CComponent(origin)
//	, m_BloomPower(origin.m_BloomPower)
//	, m_BlurRadius(origin.m_BlurRadius)
//	, m_BrightnessScale(origin.m_BrightnessScale)
//	, m_BloomThreshold(origin.m_BloomThreshold)
//	, m_Radius(origin.m_Radius)
//	, m_Sofeness(origin.m_Sofeness)
//	, m_FocusDistance(origin.m_FocusDistance)
//	, m_FocusRange(origin.m_FocusRange)
//{
//	
//}
//
//CPostProcess::~CPostProcess()
//{
//
//}
//
//void CPostProcess::Binding()
//{
//	
//}
//
//void CPostProcess::Begin()
//{
//	
//}
//
//void CPostProcess::FinalTick()
//{
//	//m_PostMaterial[0]->SetScalarParam(FLOAT_0, m_BloomPower);
//	//m_PostMaterial[0]->SetScalarParam(FLOAT_1, m_BlurRadius);
//	//m_PostMaterial[0]->SetScalarParam(FLOAT_2, m_BrightnessScale);
//	//m_PostMaterial[0]->SetScalarParam(FLOAT_3, m_BloomThreshold);
//	//
//	//m_PostMaterial[1]->SetScalarParam(FLOAT_0, m_Radius);
//	m_PostMaterial[1]->SetScalarParam(FLOAT_1, 5000.f);
//	//
//	//m_PostMaterial[2]->SetScalarParam(FLOAT_0, m_FocusDistance);
//	//m_PostMaterial[2]->SetScalarParam(FLOAT_1, m_FocusRange);
//}
//
//void CPostProcess::SaveToLevelFile(FILE* file)
//{
//
//}
//
//void CPostProcess::LoadFromLevelFile(FILE* file)
//{
//
//}