#pragma once
#include "CComputeShader.h"

class CCopyBoneShader : public CComputeShader
{
private:
	CStructuredBuffer* m_SrcBuffer;
	CStructuredBuffer* m_DestBuffer;

public:
	// g_int_0 : Bone Count
	void SetBoneCount(UINT iBoneCount) { m_Const.iArr[0] = iBoneCount; }

	// g_int_1 : Row Index
	void SetRowIndex(UINT iRowIndex) { m_Const.iArr[1] = iRowIndex; }

	void SetSourceBuffer(CStructuredBuffer* pBuffer) { m_SrcBuffer = pBuffer; }
	void SetDestBuffer(CStructuredBuffer* pBuffer) { m_DestBuffer = pBuffer; }

public:
	virtual int Binding() override;
	virtual void CalculateGroupNum() override;
	virtual void Clear() override;

public:
	CCopyBoneShader();
	~CCopyBoneShader();
};