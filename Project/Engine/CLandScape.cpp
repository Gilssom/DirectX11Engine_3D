#include "pch.h"
#include "CLandScape.h"

#include "CKeyManager.h"

#include "CTransform.h"
#include "CMaterial.h"
#include "CMesh.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
	, m_BrushScale(Vec2(0.1f, 0.1f))
	, m_IsHeightMapCreated(false)
{
	Init();

	SetFrustumCheck(false);
}

CLandScape::~CLandScape()
{

}

void CLandScape::FinalTick()
{
	if (m_IsHeightMapCreated && KEY_PRESSED(KEY::LBTN))
	{
		m_HeightMapCS->SetBrushPos(Vec2(0.5f, 0.5f));
		m_HeightMapCS->SetBrushScale(m_BrushScale);
		m_HeightMapCS->SetHeightMap(m_HeightMap);
		m_HeightMapCS->Execute();
	}
}

void CLandScape::Render()
{
	Binding();

	GetMesh()->Render();
}

void CLandScape::Binding()
{
	// 위치 정보
	Transform()->Binding();

	// 재질 정보 (지형에 대한 데이터 전달)
	GetMaterial()->SetScalarParam(INT_0, m_FaceX);
	GetMaterial()->SetScalarParam(INT_1, m_FaceZ);
	GetMaterial()->SetTexParam(TEX_0, m_HeightMap);

	GetMaterial()->Binding();
}