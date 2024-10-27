#pragma once
#include "CStructuredBuffer.h"
#include "CTexture.h"

class CGrassInstanceBuffer : public CSingleton<CGrassInstanceBuffer>
{
private:
    CStructuredBuffer*  m_InstanceBuffer;   // 풀 인스턴스를 위한 구조화 버퍼
    UINT                m_InstanceCount;    // 생성된 풀 인스턴스의 개수

public:
    void GenerateInstances(Ptr<CTexture> heightMap, UINT faceX, UINT faceZ);
    void Render();

public:
    CGrassInstanceBuffer();
    ~CGrassInstanceBuffer();
};