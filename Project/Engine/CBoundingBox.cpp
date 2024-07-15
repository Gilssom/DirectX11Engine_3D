#include "pch.h"
#include "CBoundingBox.h"

#include "CRenderManager.h"
#include "CGameObject.h"
#include "CTransform.h"

CBoundingBox::CBoundingBox()
	: CComponent(COMPONENT_TYPE::BOUNDINGBOX)
	, m_Radius(0.f)
	, m_Absolute(false)
{

}

CBoundingBox::~CBoundingBox()
{

}

void CBoundingBox::FinalTick()
{
	// Local Scale
	Matrix matScale = XMMatrixScaling(m_Radius, m_Radius, m_Radius);

	// Local Translation
	Matrix matTranslation = XMMatrixTranslation(m_OffsetPos.x, m_OffsetPos.y, m_OffsetPos.z);

	Vec3 ObjectPos = Transform()->GetWorldPos();
	Matrix matObjectTrans = XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z);

	Vec3 ObjectScale = Transform()->GetWorldScale();

	float maxScale = ObjectScale.x;
	if (maxScale < ObjectScale.y)
		maxScale = ObjectScale.y;
	if (maxScale < ObjectScale.z)
		maxScale = ObjectScale.z;

	Matrix matObejctScale = XMMatrixScaling(maxScale, maxScale, maxScale);

	if(m_Absolute)
		m_WorldMat = matScale * matTranslation * matObjectTrans;
	else
		m_WorldMat = matScale * matTranslation * matObejctScale * matObjectTrans;

	CRenderManager::GetInst()->ShowBoundingBox(true);
	DrawDebugSphere(m_WorldMat.Translation(), m_WorldMat._11, Vec4(0.f, 0.f, 1.f, 1.f), true, 0.f);
}

void CBoundingBox::SaveToLevelFile(FILE* file)
{

}

void CBoundingBox::LoadFromLevelFile(FILE* file)
{

}