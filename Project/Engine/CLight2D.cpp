#include "pch.h"
#include "CLight2D.h"

#include "CRenderManager.h"

#include "CTransform.h"

CLight2D::CLight2D()
	: CComponent(COMPONENT_TYPE::LIGHT2D)
	, m_LightIdx(-1)
{
	
}

CLight2D::~CLight2D()
{

}

void CLight2D::SetLightType(LIGHT_TYPE type)
{
	m_Info.LightType = (UINT)type;
}

void CLight2D::FinalTick()
{
	m_Info.WorldPos = Transform()->GetWorldPos();
	m_Info.WorldDir = Transform()->GetWorldDir(DIR_TYPE::RIGHT);

	m_LightIdx = CRenderManager::GetInst()->RegisterLight2D(this);
}

void CLight2D::SaveToLevelFile(FILE* file)
{
	fwrite(&m_Info, sizeof(tLightInfo), 1, file);
}

void CLight2D::LoadFromLevelFile(FILE* file)
{
	fread(&m_Info, sizeof(tLightInfo), 1, file);
}
