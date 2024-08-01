#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CWeightMapCS : public CComputeShader
{
private:
    CStructuredBuffer*  m_WeightMap;
    Ptr<CTexture>       m_BrushTex;
    CStructuredBuffer*  m_RaycastOut;
    Vec2                m_BrushScale;
    int                 m_WeightIdx;

    UINT                m_WeightMapWidth;
    UINT                m_WeightMapHeight;

public:
    void SetBrushPos(CStructuredBuffer* buffer) { m_RaycastOut = buffer; }
    void SetBrushScale(Vec2 scale) { m_BrushScale = scale; }
    void SetWeightIdx(int idx) { m_WeightIdx = idx; }
    void SetWeightMapWidthHeight(UINT width, UINT height)
    {
        m_WeightMapWidth = width;
        m_WeightMapHeight = height;
    }
    void SetWeightMap(CStructuredBuffer* weightMap) { m_WeightMap = weightMap; }
    void SetBrushTex(Ptr<CTexture> brushTex) { m_BrushTex = brushTex; }

public:
    virtual int Binding() override;
    virtual void CalculateGroupNum()override;
    virtual void Clear() override;

public:
    CWeightMapCS();
    ~CWeightMapCS();
};