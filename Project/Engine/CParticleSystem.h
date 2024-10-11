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

	bool					m_IsPlayed;			// 재생 여부


public:
	void SetParticleTexture(Ptr<CTexture> tex) { m_ParticleTex = tex; }
	void SetMaxParticleCount(UINT max);

	void SetSpawnRate(UINT rate);
	void SetLife(float minLife, float maxLife);
	void SetScale(const Vec3& minScale, const Vec3& maxScale);
	void SetVelocity(Vec3 dir, float minSpeed, float maxSpeed);
	void SetColor(Vec4 spawnColor, Vec3 endColor);
	void SetFadeInOut(bool isFadeOut, float startRatio);
	void SetModuleOnOff(PARTICLE_MODULE module, bool onoff);

	void SetIsPlayed(bool play) { m_IsPlayed = play; }
 
public:
	Ptr<CTexture> GetParticleTexture() { return m_ParticleTex; }
	tParticleModule& GetModule() { return m_Module; }
	bool GetIsPlayed() { return m_IsPlayed; }
	void UpdateModuleBuffer();

private:
	void CalculateSpawnCount();
	bool CheckParticleEnd();

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

