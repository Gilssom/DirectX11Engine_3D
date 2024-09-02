#pragma once
#include "Singletone.h"

#include "CStructuredBuffer.h"
#include "CTexture.h"
#include "CCopyBoneShader.h"

class CInstancingBuffer : public CSingleton<CInstancingBuffer>
{
private:
	ComPtr<ID3D11Buffer>		m_InstancingBuffer;
	UINT						m_MaxCount;
	vector<tInstancingData>		m_vecData;

	// anim 3D ¿ëµµ
	vector<CStructuredBuffer*>	m_vecBoneMat;
	int							m_AnimInstCount;
	CStructuredBuffer*			m_BoneBuffer;

	Ptr<CCopyBoneShader>		m_CopyShader;

public:
	void Init();
	void Clear()
	{
		m_vecData.clear();
		m_vecBoneMat.clear();
		m_AnimInstCount = 0;
	}

	void AddInstancingData(tInstancingData& tData) { m_vecData.push_back(tData); }

	UINT GetInstanceCount() { return (UINT)m_vecData.size(); }
	ComPtr<ID3D11Buffer> GetBuffer() { return m_InstancingBuffer; }
	void SetData();

	void AddInstancingBoneMat(CStructuredBuffer* pBuffer);
	int GetAnimInstancingCount() { return m_AnimInstCount; }
	CStructuredBuffer* GetBoneBuffer() { return m_BoneBuffer; }

private:
	void Resize(UINT iCount);

public:
	CInstancingBuffer();
	~CInstancingBuffer();
};