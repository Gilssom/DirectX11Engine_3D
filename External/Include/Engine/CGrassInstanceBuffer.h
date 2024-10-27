#pragma once
#include "CStructuredBuffer.h"
#include "CTexture.h"

class CGrassInstanceBuffer : public CSingleton<CGrassInstanceBuffer>
{
private:
    CStructuredBuffer*  m_InstanceBuffer;   // Ǯ �ν��Ͻ��� ���� ����ȭ ����
    UINT                m_InstanceCount;    // ������ Ǯ �ν��Ͻ��� ����

public:
    void GenerateInstances(Ptr<CTexture> heightMap, UINT faceX, UINT faceZ);
    void Render();

public:
    CGrassInstanceBuffer();
    ~CGrassInstanceBuffer();
};