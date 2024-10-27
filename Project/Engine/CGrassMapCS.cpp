#include "pch.h"
#include "CGrassMapCS.h"
#include "CStructuredBuffer.h"

CGrassMapCS::CGrassMapCS()
	: CComputeShader(32, 32, 1, L"shader\\grassmap.fx", "CS_GrassMap")
    , m_GrassInstances(nullptr)
    , m_RaycastOut(nullptr)
    , m_GrassMapWidth(0)
    , m_GrassMapHeight(0)
{

}

CGrassMapCS::~CGrassMapCS()
{

}

int CGrassMapCS::Binding()
{
    if (nullptr == m_GrassInstances || nullptr == m_BrushTex || nullptr == m_RaycastOut
        || 0 == m_GrassMapWidth || 0 == m_GrassMapHeight)
        return E_FAIL;

    m_Const.iArr[0] = m_GrassMapWidth;
    m_Const.iArr[1] = m_GrassMapHeight;
    m_Const.v2Arr[0] = m_BrushScale;

    m_GrassInstances->Binding_CS_UAV(0);
    m_RaycastOut->Binding_CS_SRV(20);
    m_BrushTex->Binding_CS_SRV(0);

    return S_OK;
}

void CGrassMapCS::CalculateGroupNum()
{
    m_GroupX = m_GrassMapWidth / m_ThreadPerGroupX;
    m_GroupY = m_GrassMapHeight / m_ThreadPerGroupY;

    if (0 < (UINT)m_GrassMapWidth % m_ThreadPerGroupX)
        m_GroupX += 1;

    if (0 < (UINT)m_GrassMapHeight % m_ThreadPerGroupY)
        m_GroupY += 1;
}

void CGrassMapCS::Clear()
{
    m_BrushTex->Clear_CS_SRV(0);
    m_GrassInstances->Clear_CS_UAV();
    m_RaycastOut->Clear_CS_SRV();
}