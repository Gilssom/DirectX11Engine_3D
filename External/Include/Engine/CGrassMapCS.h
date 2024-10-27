#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CGrassMapCS : public CComputeShader
{
private:
    CStructuredBuffer*  m_GrassInstances;   // 풀 인스턴스 버퍼
    CStructuredBuffer*  m_RaycastOut;       // Raycasting 결과
    Vec2                m_BrushScale;       // 브러시 크기
    Ptr<CTexture>       m_BrushTex;         // 브러시 텍스처 (Brush Texture)
    UINT                m_GrassMapWidth;    // 풀 배치 맵의 너비
    UINT                m_GrassMapHeight;   // 풀 배치 맵의 높이
    UINT                m_GrassCount;

public:
    void SetBrushPos(CStructuredBuffer* buffer) { m_RaycastOut = buffer; }
    void SetBrushScale(Vec2 scale) { m_BrushScale = scale; }
    void SetGrassCount(UINT count) { m_GrassCount = count; }
    void SetGrassMapWidthHeight(UINT width, UINT height)
    {
        m_GrassMapWidth = width;
        m_GrassMapHeight = height;
    }
    void SetGrassInstances(CStructuredBuffer* grassInstances) { m_GrassInstances = grassInstances; }
    void SetBrushTex(Ptr<CTexture> brushTex) { m_BrushTex = brushTex; }

public:
    virtual int Binding() override;
    virtual void CalculateGroupNum() override;
    virtual void Clear() override;

public:
    CGrassMapCS();
    ~CGrassMapCS();
};