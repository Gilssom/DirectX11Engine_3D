#include "pch.h"
#include "CLight3D.h"

#include "CRenderManager.h"
#include "CTransform.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_Info{}
	, m_LightIdx(-1)
{

}

CLight3D::~CLight3D()
{

}

void CLight3D::SetLightType(LIGHT_TYPE type)
{
	m_Info.LightType = (UINT)type;
}

void CLight3D::FinalTick()
{
	m_Info.WorldPos = Transform()->GetWorldPos();
	m_Info.WorldDir = Transform()->GetWorldDir(DIR_TYPE::FRONT); // 3D 공간의 정면 방향

	m_LightIdx = CRenderManager::GetInst()->RegisterLight3D(this);
}

void CLight3D::SaveToLevelFile(FILE* file)
{
	fwrite(&m_Info, sizeof(tLightInfo), 1, file);
}

void CLight3D::LoadFromLevelFile(FILE* file)
{
	fread(&m_Info, sizeof(tLightInfo), 1, file);
}