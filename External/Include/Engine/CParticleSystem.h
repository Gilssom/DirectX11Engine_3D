#pragma once
#include "CRenderComponent.h"
#include "CParticleTickCS.h"

class CStructuredBuffer;

class CParticleSystem : public CRenderComponent
{
private:
	Ptr<CParticleTickCS>	m_TickCS;
	CStructuredBuffer*		m_ParticleBuffer;	// 구조화 버퍼로 GPU 메모리 전달 용도
	CStructuredBuffer*		m_SpawnCountBuffer;	// Spawn Count 정보 전달 용도
	CStructuredBuffer*		m_ModuleBuffer;		// Module 정보 전달 용도

	Ptr<CTexture>			m_ParticleTex;		// 파티클 입자 텍스처

	float					m_Time;				// 파티클 생성 누적 시간
	float					m_BurstTime;		// Spawn Burst 체크용 변수

	// Particle Data
	UINT					m_MaxParticle;		// 파티클 최대 개수

	tParticleModule			m_Module;			// 파티클의 기능 정의


public:
	void SetParticleTexture(Ptr<CTexture> tex) { m_ParticleTex = tex; }
	void SetMaxParticleCount(UINT max);

private:
	void CalculateSpawnCount();

public:
	virtual void FinalTick() override;
	virtual void Render() override;

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CParticleSystem);

public:
	CParticleSystem();
	CParticleSystem(const CParticleSystem& other);
	~CParticleSystem();

};

