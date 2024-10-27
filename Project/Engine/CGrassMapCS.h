#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CGrassMapCS : public CComputeShader
{
private:
    CStructuredBuffer*  m_GrassInstances;   // Ǯ �ν��Ͻ� ����
    CStructuredBuffer*  m_RaycastOut;       // Raycasting ���
    Vec2                m_BrushScale;       // �귯�� ũ��
    Ptr<CTexture>       m_BrushTex;         // �귯�� �ؽ�ó (Brush Texture)
    UINT                m_GrassMapWidth;    // Ǯ ��ġ ���� �ʺ�
    UINT                m_GrassMapHeight;   // Ǯ ��ġ ���� ����
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