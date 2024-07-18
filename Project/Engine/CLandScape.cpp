#include "pch.h"
#include "CLandScape.h"

#include "CTransform.h"
#include "CMaterial.h"
#include "CMesh.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
{
	Init();

	SetFrustumCheck(false);
}

CLandScape::~CLandScape()
{

}

void CLandScape::FinalTick()
{

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