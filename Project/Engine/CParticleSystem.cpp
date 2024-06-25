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
	// Particle Tick 용도 Compute Shader
	m_TickCS = (CParticleTickCS*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"ParticleTickCS").Get();


	// Particle System 이 사용할 Mesh 와 Material 설정
	SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"PointMesh")); // 정점 하나를 기준으로 Geometry Shader 로 보낸다.
	SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"ParticleMaterial"));


	// Particle 정보를 저장할 구조화 버퍼 생성
	// Tick 이 생겼으니 데이터를 강제로 세팅할 필요도 없음
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

	// GPU 간의 복사 과정이 걸리긴 하지만 별로 안걸리니까 그냥 써라
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
	// 원래 CPU 에서 연산을 하게 되면 여기서 몇천 몇만번의 반복문이 돌아져야 한다.
	// 이 일을 GPU 에서 처리를 하도록 설계를 한다.

	// Spawn Count 계산
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

	GetMaterial()->SetTexParam(TEX_0, m_ParticleTex);
	GetMaterial()->Binding();

	// Instance 로 Render 시 Inatance 고유 ID(몇번째) 를 자동으로 넘겨줌
	// 모든 입자들을 반복문을 통해 Render 할 때보다 인스턴싱이 훨씬 더 프레임 방어도 잘되고 효율적임
	GetMesh()->Render_Particle(m_MaxParticle);

	// Clear
	m_ParticleBuffer->Clear_SRV();
	m_ModuleBuffer->Clear_SRV();
}

void CParticleSystem::SetMaxParticleCount(UINT max)
{
	m_MaxParticle = max;

	if (m_ParticleBuffer->GetElementCount() < m_MaxParticle)
	{
		m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticle, SB_TYPE::SRV_UAV, false);
	}
}

void CParticleSystem::CalculateSpawnCount()
{
	m_Time += DT;
	tSpawnCount count = { };

	// Spawn Module
	if (m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN])
	{
		// Spawn Rate (이번 Tick 에서 파티클 하나가 생성될 시간)
		float Term = 1.f / (float)m_Module.vSpawnRate;
		UINT SpawnCount = 0;	// SpawnCount Per Tick

		// 시간이 Term 을 넘을 경우를 대비해서 추가 연산 (1초에 10000개 생성같은 경우)
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

	// SpawnCount Buffer 전달
	m_SpawnCountBuffer->SetData(&count);
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