#include "pch.h"
#include "CLandScape.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
{
	Init();
}

CLandScape::~CLandScape()
{

}

void CLandScape::FinalTick()
{

}

void CLandScape::Render()
{

}