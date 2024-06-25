#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CParticleTickCS : public CComputeShader
{
private:
	CStructuredBuffer* m_ParticleBuffer;
	CStructuredBuffer* m_SpawnCountBuffer;
	CStructuredBuffer* m_ModuleBuffer;
	Ptr<CTexture>	   m_NoiseTex;

	Vec3			   m_vParticleWorldPos;

public:
	void SetParticleModuleBuffer(CStructuredBuffer* buffer) { m_ModuleBuffer = buffer; }
	void SetParticleBuffer(CStructuredBuffer* buffer) { m_ParticleBuffer = buffer; }
	void SetSpawnCount(CStructuredBuffer* buffer) { m_SpawnCountBuffer = buffer; }
	void SetParticleWorldPos(Vec3 pos) { m_vParticleWorldPos = pos; }

public:
	virtual int Binding() override;
	virtual void CalculateGroupNum() override;
	virtual void Clear() override;


public:
	CParticleTickCS();
	~CParticleTickCS();
};

