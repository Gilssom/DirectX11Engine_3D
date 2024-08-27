#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CAnimation3DShader : public CComputeShader
{
private:
    CStructuredBuffer* m_pFrameDataBuffer;			// t16
    CStructuredBuffer* m_pOffsetMatBuffer;			// t17 
    CStructuredBuffer* m_pOutputBuffer;				// u0


public:
    // g_int_0 : BonCount, g_int_1 : Frame Index
    void SetBoneCount(int iBoneCount) { m_Const.iArr[0] = iBoneCount; }
    void SetFrameIndex(int iFrameIdx) { m_Const.iArr[1] = iFrameIdx; }
    void SetNextFrameIdx(int iFrameIdx) { m_Const.iArr[2] = iFrameIdx; }
    void SetFrameRatio(float fFrameRatio) { m_Const.fArr[0] = fFrameRatio; }
    void SetFrameDataBuffer(CStructuredBuffer* buffer) { m_pFrameDataBuffer = buffer; }
    void SetOffsetMatBuffer(CStructuredBuffer* buffer) { m_pOffsetMatBuffer = buffer; }
    void SetOutputBuffer(CStructuredBuffer* buffer) { m_pOutputBuffer = buffer; }

public:
    virtual int Binding() override;
    virtual void CalculateGroupNum() override;
    virtual void Clear() override;

public:
    CAnimation3DShader();
    ~CAnimation3DShader();
};