#pragma once
#include "CRenderComponent.h"
#include "CParticleTickCS.h"

class CStructuredBuffer;

class CParticleSystem : public CRenderComponent
{
private:
	Ptr<CParticleTickCS>	m_TickCS;
	CStructuredBuffer*		m_ParticleBuffer;	// ����ȭ ���۷� GPU �޸� ���� �뵵
	CStructuredBuffer*		m_SpawnCountBuffer;	// Spawn Count ���� ���� �뵵
	CStructuredBuffer*		m_ModuleBuffer;		// Module ���� ���� �뵵

	Ptr<CTexture>			m_ParticleTex;		// ��ƼŬ ���� �ؽ�ó

	float					m_Time;				// ��ƼŬ ���� ���� �ð�
	float					m_BurstTime;		// Spawn Burst üũ�� ����

	// Particle Data
	UINT					m_MaxParticle;		// ��ƼŬ �ִ� ����

	tParticleModule			m_Module;			// ��ƼŬ�� ��� ����


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

public:
	tParticleModule& GetModule() { return m_Module; }
	void UpdateModuleBuffer();

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

