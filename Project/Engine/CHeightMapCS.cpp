#include "pch.h"
#include "CHeightMapCS.h"

CHeightMapCS::CHeightMapCS()
    : CComputeShader(32, 32, 1, L"shader\\heightmap.fx", "CS_HeightMap")
{

}

CHeightMapCS::~CHeightMapCS()
{

}

int CHeightMapCS::Binding()
{
    if (m_HeightMapTex == nullptr)
        return E_FAIL;

    m_HeightMapTex->Binding_CS_UAV(0);

    m_Const.iArr[0] = (UINT)m_HeightMapTex->GetWidth();
    m_Const.iArr[1] = (UINT)m_HeightMapTex->GetHeight();

    m_Const.v2Arr[0] = m_BrushPos;
    m_Const.v2Arr[1] = m_BrushScale;

    if (nullptr != m_BrushTex)
    {
        m_BrushTex->Binding_CS_SRV(0);
        m_Const.btex[0] = 1;
    }
    else
        m_Const.btex[0] = 0;

    return S_OK;
}

void CHeightMapCS::CalculateGroupNum()
{
    m_GroupX = ((UINT)m_HeightMapTex->GetWidth() / m_ThreadPerGroupX);
    m_GroupY = ((UINT)m_HeightMapTex->GetHeight() / m_ThreadPerGroupY);
    m_GroupZ = 1;

    if (0 < (UINT)m_HeightMapTex->GetWidth() % m_ThreadPerGroupX)
        m_GroupX += 1;

    if (0 < (UINT)m_HeightMapTex->GetHeight() % m_ThreadPerGroupY)
        m_GroupY += 1;
}

void CHeightMapCS::Clear()
{
    m_HeightMapTex->Clear_CS_UAV(0);
    m_HeightMapTex = nullptr;

    if (nullptr != m_BrushTex)
    {
        m_BrushTex->Clear_CS_SRV(0);
        m_BrushTex = nullptr;
    }
}