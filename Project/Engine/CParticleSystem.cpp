#include "pch.h"
#include "CParticleSystem.h"

#include "CTransform.h"

#include "CDevice.h"
#include "CStructuredBuffer.h"

#include "CTimeManager.h"

CParticleSystem::CParticleSystem()
	: CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_ParticleBuffer(nullptr)
	, m_Time(0.f)
	, m_BurstTime(0.f)
	, m_MaxParticle(10000)
{
	SetFrustumCheck(false);
	SetDynamicShadow(false);

	// Particle Tick �뵵 Compute Shader
	m_TickCS = (CParticleTickCS*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"ParticleTickCS").Get();


	// Particle System �� ����� Mesh �� Material ����
	SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"PointMesh")); // ���� �ϳ��� �������� Geometry Shader �� ������.
	SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"ParticleMaterial"), 0);


	// Particle ������ ������ ����ȭ ���� ����
	// Tick �� �������� �����͸� ������ ������ �ʿ䵵 ����
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticle, SB_TYPE::SRV_UAV, false);

	m_SpawnCountBuffer = new CStructuredBuffer;
	m_SpawnCountBuffer->Create(sizeof(tSpawnCount), 1, SB_TYPE::SRV_UAV, true);


	// Particle Module Setting Test
	// Spawn Module
	m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN] = true;
	m_Module.vSpawnRate = 100;
	m_Module.vSpawnColor = Vec4(0.3f, 0.f, 0.3f, 1.f);
	m_Module.MinLife = 4.f;
	m_Module.MaxLife = 8.f;
	m_Module.vSpawnMinScale = Vec3(50.f, 50.f, 1.f);
	m_Module.vSpawnMaxScale = Vec3(75.f, 75.f, 1.f);

	// Spawn Area (No Module)
	m_Module.SpawnShape = 0;
	m_Module.SpawnShapeScale.y = 2500.f;

	// Don't Spawn Area
	m_Module.BlockSpawnShape = 1;
	m_Module.BlockSpawnShapeScale.x = 0.f;

	// Local || World Space Setting
	m_Module.SpaceType = 1;


	// Spawn Burst Module
	m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN_BURST] = false;
	m_Module.SpawnBurstRepeat = true;
	m_Module.SpawnBurstCount = 100;
	m_Module.SpawnBurstRepeatTime = 0.5f;

	
	// Add Velocity Module
	m_Module.Module[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = true;
	m_Module.AddVelocityType = 3;
	m_Module.AddVelocityFixedDir = Vec3(1.f, 0.f, 0.f);
	m_Module.AddMinSpeed = 20.f;
	m_Module.AddMaxSpeed = 60.f;


	// Scale Module
	m_Module.Module[(UINT)PARTICLE_MODULE::SCALE] = true;
	m_Module.StartScale = 1.f;
	m_Module.EndScale = 2.f;


	// Drag Module
	m_Module.Module[(UINT)PARTICLE_MODULE::DRAG] = true;
	m_Module.DestNormalizedAge = 0.8f;
	m_Module.LimitSpeed = 10.f;


	// Noise Force Module
	m_Module.Module[(UINT)PARTICLE_MODULE::NOISE_FORCE] = false;
	m_Module.NoiseForceTerm = 0.3f;
	m_Module.NoiseForceScale = 1.f;


	// Render Module
	m_Module.Module[(UINT)PARTICLE_MODULE::RENDER] = true;
	m_Module.EndColor = Vec3(1.f, 0.6f, 1.f);
	m_Module.FadeOut = true;
	m_Module.FadeOutStartRatio = 0.2f;
	m_Module.VelocityAlignment = false;

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tParticleModule) + (16 % (16 - sizeof(tParticleModule) % 16)), 1, SB_TYPE::SRV_UAV, true, &m_Module);
}

CParticleSystem::CParticleSystem(const CParticleSystem& other)
	: CRenderComponent(other)
	, m_TickCS(other.m_TickCS)
	, m_ParticleBuffer(nullptr)
	, m_SpawnCountBuffer(nullptr)
	, m_ModuleBuffer(nullptr)
	, m_ParticleTex(other.m_ParticleTex)
	, m_Time(0.f)
	, m_BurstTime(0.f)
	, m_MaxParticle(other.m_MaxParticle)
	, m_Module(other.m_Module)
{
	assert(other.m_ParticleBuffer && other.m_SpawnCountBuffer && other.m_ModuleBuffer);

	// GPU ���� ���� ������ �ɸ��� ������ ���� �Ȱɸ��ϱ� �׳� ���
	m_ParticleBuffer = new CStructuredBuffer(*other.m_ParticleBuffer);
	m_SpawnCountBuffer = new CStructuredBuffer(*other.m_SpawnCountBuffer);
	m_ModuleBuffer = new CStructuredBuffer(*other.m_ModuleBuffer);
}

CParticleSystem::~CParticleSystem()
{
	delete m_ParticleBuffer;
	delete m_SpawnCountBuffer;
	delete m_ModuleBuffer;
}

void CParticleSystem::FinalTick()
{
	// ���� CPU ���� ������ �ϰ� �Ǹ� ���⼭ ��õ ����� �ݺ����� �������� �Ѵ�.
	// �� ���� GPU ���� ó���� �ϵ��� ���踦 �Ѵ�.

	// Spawn Count ���
	CalculateSpawnCount();

	// Compute Shader Excecute
	m_TickCS->SetParticleWorldPos(Transform()->GetWorldPos());
	m_TickCS->SetParticleModuleBuffer(m_ModuleBuffer);
	m_TickCS->SetParticleBuffer(m_ParticleBuffer);
	m_TickCS->SetSpawnCount(m_SpawnCountBuffer);
	
	if (FAILED(m_TickCS->Execute()))
	{
		assert(nullptr);
	}
}

void CParticleSystem::Render()
{
	// Test Particle Render
	m_ParticleBuffer->Binding(17);
	m_ModuleBuffer->Binding(18);
	Transform()->Binding();

	GetMaterial(0)->SetTexParam(TEX_0, m_ParticleTex);
	GetMaterial(0)->Binding();

	// Instance �� Render �� Inatance ���� ID(���°) �� �ڵ����� �Ѱ���
	// ��� ���ڵ��� �ݺ����� ���� Render �� ������ �ν��Ͻ��� �ξ� �� ������ �� �ߵǰ� ȿ������
	GetMesh()->Render_Particle(m_MaxParticle);

	// Clear
	m_ParticleBuffer->Clear();
	m_ModuleBuffer->Clear();
}

void CParticleSystem::SetMaxParticleCount(UINT max)
{
	m_MaxParticle = max;

	if (m_ParticleBuffer->GetElementCount() < m_MaxParticle)
	{
		m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticle, SB_TYPE::SRV_UAV, false);
	}
}

void CParticleSystem::SetSpawnRate(UINT rate)
{
	m_Module.vSpawnRate = rate;
	UpdateModuleBuffer();
}

void CParticleSystem::SetLife(float minLife, float maxLife)
{
	m_Module.MinLife = minLife;
	m_Module.MaxLife = maxLife;
	UpdateModuleBuffer();
}

void CParticleSystem::SetScale(const Vec3& minScale, const Vec3& maxScale)
{
	m_Module.vSpawnMinScale = minScale;
	m_Module.vSpawnMaxScale = maxScale;
	UpdateModuleBuffer();
}

void CParticleSystem::SetVelocity(Vec3 dir, float minSpeed, float maxSpeed)
{
	m_Module.AddVelocityFixedDir = dir;
	m_Module.AddMinSpeed = minSpeed;
	m_Module.AddMaxSpeed = maxSpeed;
	UpdateModuleBuffer();
}

void CParticleSystem::SetColor(Vec4 spawnColor, Vec3 endColor)
{
	m_Module.vSpawnColor = spawnColor;
	m_Module.EndColor = endColor;
	UpdateModuleBuffer();
}

void CParticleSystem::SetFadeInOut(bool isFadeOut, float startRatio)
{
	m_Module.FadeOut = isFadeOut;
	m_Module.FadeOutStartRatio = startRatio;
	UpdateModuleBuffer();
}

void CParticleSystem::SetModuleOnOff(PARTICLE_MODULE module, bool onoff)
{
	m_Module.Module[(UINT)module] = onoff;
	UpdateModuleBuffer();
}

void CParticleSystem::CalculateSpawnCount()
{
	m_Time += DT;
	tSpawnCount count = { };

	// Spawn Module
	if (m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN])
	{
		// Spawn Rate (�̹� Tick ���� ��ƼŬ �ϳ��� ������ �ð�)
		float Term = 1.f / (float)m_Module.vSpawnRate;
		UINT SpawnCount = 0;	// SpawnCount Per Tick

		// �ð��� Term �� ���� ��츦 ����ؼ� �߰� ���� (1�ʿ� 10000�� �������� ���)
		if (Term < m_Time)
		{
			float Value = m_Time / Term;
			SpawnCount = (UINT)Value;
			m_Time -= (float)SpawnCount * Term;
		}

		count.SpawnCount = SpawnCount;
	}

	if (m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN_BURST])
	{
		UINT BurstCount = 0;

		if (m_BurstTime == 0.f)
		{
			BurstCount = m_Module.SpawnBurstCount;
		}

		m_BurstTime += DT;

		if (m_Module.SpawnBurstRepeat && m_Module.SpawnBurstRepeatTime <= m_BurstTime)
		{
			m_BurstTime -= m_Module.SpawnBurstRepeatTime;
			BurstCount += m_Module.SpawnBurstCount;
		}

		count.SpawnCount += BurstCount;
	}

	// SpawnCount Buffer ����
	m_SpawnCountBuffer->SetData(&count);
}

void CParticleSystem::UpdateModuleBuffer()
{
	if (m_ModuleBuffer)
	{
		delete m_ModuleBuffer;
		m_ModuleBuffer = nullptr;
	}

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tParticleModule) + (16 % (16 - sizeof(tParticleModule) % 16)), 1, SB_TYPE::SRV_UAV, true, &m_Module);
}

void CParticleSystem::SaveToLevelFile(FILE* file)
{
	CRenderComponent::SaveToLevelFile(file);

	SaveAssetRef<CComputeShader>((CComputeShader*)m_TickCS.Get(), file);
	SaveAssetRef(m_ParticleTex, file);

	fwrite(&m_MaxParticle, sizeof(UINT), 1, file);
	fwrite(&m_Module, sizeof(tParticleModule), 1, file);
}

void CParticleSystem::LoadFromLevelFile(FILE* file)
{
	CRenderComponent::LoadFromLevelFile(file);

	Ptr<CComputeShader> pTickCs = m_TickCS.Get();
	LoadAssetRef(pTickCs, file);
	m_TickCS = (CParticleTickCS*)pTickCs.Get();

	LoadAssetRef(m_ParticleTex, file);

	fread(&m_MaxParticle, sizeof(UINT), 1, file);
	SetMaxParticleCount(m_MaxParticle);

	fread(&m_Module, sizeof(tParticleModule), 1, file);
}