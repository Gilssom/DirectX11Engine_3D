#ifndef _PARTICLE_TICK
#define _PARTICLE_TICK

#include "value.fx"
#include "func.fx"


// Tick �� ȣ�� ���� Particle ���� Binding �� ��
RWStructuredBuffer<tParticle>   ParticleBuffer   : register(u0);
RWStructuredBuffer<tSpawnCount> SpawnCountBuffer : register(u1);
Texture2D                       NoiseTex         : register(t20);
StructuredBuffer<tParticleModule> Module         : register(t21);


// Particle Buffer �� ����
#define ParticleBufferSize  g_int_0

// Particle Object �� ��ġ
#define ParticleObjectPos   g_vec4_0

// Particle Tick Buffer ����
#define Particle            ParticleBuffer[_ID.x]

// Spawn Count Buffer ����
#define SpawnCount          SpawnCountBuffer[0].spawnCount

// Module Check
#define SpawnModule         Module[0].Module[0]
#define SpawnBurstModule    Module[0].Module[1]
#define SpawnShapeType      Module[0].SpawnShape
#define AddVelocityModule   Module[0].Module[2]
#define ScaleModule         Module[0].Module[3]
#define DragModule          Module[0].Module[4]
#define NoiseForceModule    Module[0].Module[5]
#define RenderModule        Module[0].Module[6]

// �����ϰ� ������ ��ó�ϱ� ���ؼ� ������ 32����
[numthreads(32, 1, 1)]
void CS_ParticleTick(int3 _ID : SV_DispatchThreadID)
{
    // Thread ���� ����ȭ�� �ʿ���
    // 1. ���� ó�� �̱� ������ Count �� 1�� �����ϴ� ���� �ƴ϶� 
    //   ��ü�� �ѹ��� �����ϱ� ������ �������� ����
    // 2. �׸��� �������� �������� ���� (���ÿ� ����ó���� �ϱ� ������ ���� �������� ������ ��)
    // ���� ����� �����ϴ� �Լ��� ���ο� ������
    
    // Thread ID == Particle Index
    // Thread ID �� Particle ������ �ʰ��ϸ� return
    if (ParticleBufferSize <= _ID.x)
        return;
    
    // InterlockedExchange �� �������� �ִ�.
    // �ϳ� �ϳ��� �� ���ϸ鼭 �����ϴ°� �´µ�
    // ��ΰ� 1ȸ��, 2ȸ�� ... nȸ���� �� �Բ� �����ϴ°� �ƴ� ���� �ִ�.(n�� �������� ������ ���� ���ɼ� ���)
    // �׷��� ���߿� ����Ǵ� Interlocked Thread �� ���� ����ִ� ������ �����Ű�� ������ �߻�
    
    // InterlockedCompareExchange
    // dest ���� compare value �� �ѹ� �� �񱳸� �ؼ� ���� ���� ������ �߻��� �� ���� ����
    // ��������� nȸ���� ù �õ��ϴ� Thread �� ����� �ϰ� �ȴ�. 
    // (�� ������ �̷������ ���� Compare ������ ����� �ȵ�)
    
    //��Ȱ��ȭ�� ��ƼŬ�� Ȱ��ȭ ���Ѿ���
    if (Particle.Active == 0)
    {
        // Spawn Module
        if (SpawnModule || SpawnBurstModule)
        {
            // �̹��� Ȱ��ȭ ������ ��ƼŬ �� üũ
            int CurSpawnCount = SpawnCount;
        
            // ��ƼŬ Ȱ��ȭ ���� ���ڰ� 0���� ũ��
            while (0 < CurSpawnCount)
            {
                // Spawn Count �� ����ִ� ���� ���� ����
                int OriginValue = 0;
            
                //InterlockedExchange(SpawnCount, SpawnCount - 1, OriginValue);
                // ��ü�� �õ��� ��, SpawnCount �� ����ִ� ���� ���� ���� ������ ��쿡�� ���� ��ü�Ѵ�.
                InterlockedCompareExchange(SpawnCount, CurSpawnCount, SpawnCount - 1, OriginValue);
            
                // ��ü�� ���� ���� �����ϴ� ���� ��ġ�ϸ� Ȱ��ȭ ����
                // ������ Thread �� �����ϸ� ���� Thread ���� �Ʒ� ���ǿ� ���� �� ����
                // ���� Ȱ��ȭ �õ� �ܰ�� ���� ���
                if (OriginValue == CurSpawnCount)
                {
                    // Random �Լ�ȭ
                    float3 vRandom  = GetRandom(NoiseTex, 2.f * (float) _ID.x       / (float) (ParticleBufferSize - 1));
                    float3 vRandom1 = GetRandom(NoiseTex, 2.f * (float) (_ID.x + 1) / (float) (ParticleBufferSize - 1));
                    float3 vRandom2 = GetRandom(NoiseTex, 2.f * (float) (_ID.x + 2) / (float) (ParticleBufferSize - 1));
                    
                    
                    float3 vSpawnPos = (float3) 0.f;
                    // 0 : Box,  1 : Sphere
                    if (SpawnShapeType == 0)
                    {
                        vSpawnPos.x = vRandom.x * Module[0].SpawnShapeScale.x - (Module[0].SpawnShapeScale.x / 2.f);
                        vSpawnPos.y = vRandom.y * Module[0].SpawnShapeScale.y - (Module[0].SpawnShapeScale.y / 2.f);
                        vSpawnPos.z = vRandom.z * Module[0].SpawnShapeScale.z - (Module[0].SpawnShapeScale.z / 2.f);
                    }
                    else if (SpawnShapeType == 1)
                    {
                        float fRadius = Module[0].SpawnShapeScale.x / 2.f;
                        //float fAngle = vRandom1.x * 2.f * PI;
                        //float fDist = vRandom1.y * fRadius;                       
                        float fBlockRadius = Module[0].BlockSpawnShapeScale.x / 2.f;
                        float fDifferRadius = fRadius - fBlockRadius;
                        
                        // ���� ���� ���� (Sphere) - Box �� ���� ����
                        vSpawnPos = (normalize(vRandom1 - 0.5f) * fDifferRadius * vRandom2.x)
                                    + (normalize(vRandom1 - 0.5f) * fBlockRadius);
                    }                 
                    
                    // Add Velocity Module
                    Particle.vVelocity = (float3) 0.f;
                    if (AddVelocityModule)
                    {
                        float fSpeed = Module[0].AddMinSpeed + (Module[0].AddMaxSpeed - Module[0].AddMinSpeed) * vRandom2.x;
                        
                        // Random
                        if (Module[0].AddVelocityType == 0)
                            Particle.vVelocity = normalize(vRandom2 - 0.5f) * fSpeed;

                        // From Center
                        else if (Module[0].AddVelocityType == 1)
                            Particle.vVelocity = normalize(vSpawnPos) * fSpeed;
                        
                        // To Center
                        else if (Module[0].AddVelocityType == 2)
                            Particle.vVelocity = -normalize(vSpawnPos) * fSpeed;
                        
                        // Fixed
                        else
                            Particle.vVelocity = normalize(Module[0].AddVelocityFixedDir) * fSpeed;                       
                    }
                    
                    Particle.vLocalPos = vSpawnPos;
                    Particle.vWorldPos = Particle.vLocalPos + ParticleObjectPos.xyz;                   
                    
                    // Min ~ Max Random
                    Particle.vWorldInitScale = (Module[0].vSpawnMaxScale - Module[0].vSpawnMinScale) * vRandom.x + Module[0].vSpawnMinScale;
                    //clamp(vRandom.x, Module[0].vSpawnMinScale, Module[0].vSpawnMaxScale);
                    
                    Particle.vColor = Module[0].vSpawnColor/* * float4(vRandom.x, vRandom1.y, vRandom2.z, 1.f)*/;
                    Particle.Mass = 1.f;
                    
                    Particle.Age = 0.f;
                    Particle.NormalizedAge = 0;
                    Particle.Life = (Module[0].MaxLife - Module[0].MinLife) * vRandom1.z + Module[0].MinLife;
                    //clamp(vRandom.y, Module[0].MinLife, Module[0].MaxLife);
                    Particle.Active = 1;
                
                    break;
                }
            
                // �������� ���� Thread ���� Ȱ��ȭ ������ ��ƼŬ ���� ��üũ
                CurSpawnCount = SpawnCount;
            }
        }
    }
    else
    {
        // Global Data �� Binding_CS �� ���ؼ� �޾ƿ����� Ȱ��
        
        Particle.vForce = float3(0.f, 0.f, 0.f);
        
        // Noise Force Module (Random Direction Force)
        if (NoiseForceModule)
        {
            // ���� �ð����� Noise Force �� ������ Random ���� ���Ѵ�.
            if(Particle.NoiseForceAccTime >= Module[0].NoiseForceTerm)
            {
                Particle.NoiseForceAccTime -= Module[0].NoiseForceTerm;
                
                float3 vRandom = GetRandom(NoiseTex, ((float) _ID.x / (float) (ParticleBufferSize - 1)));
                float3 vNoiseForce = normalize(vRandom.xyz - 0.5f); // Random Direction
                
                Particle.NoiseForceDir = vNoiseForce;
            }
            
            Particle.vForce += Particle.NoiseForceDir * Module[0].NoiseForceScale;
            Particle.NoiseForceAccTime += DeltaTime;
        }

        // Particle �� �־��� ���� ���� ���ӵ� ���
        float3 vAccel = Particle.vForce / Particle.Mass;
        
        // ���ӵ��� ���� �ӵ��� �ǽð� ��ȭ
        Particle.vVelocity += vAccel * DeltaTime;
        
        
        // Velocity �� ���� �̵�
        if (Module[0].SpaceType == 0)
        {
            // Local Space Type
            Particle.vLocalPos += Particle.vVelocity * DeltaTime;
            Particle.vWorldPos = Particle.vLocalPos + ParticleObjectPos.xyz;
        }
        else
        {
            // World Space Type
            Particle.vWorldPos += Particle.vVelocity * DeltaTime;
        }
            
        
        // Scale Module �� ���� ���� ũ�� ���
        Particle.vWorldCurrentScale = Particle.vWorldInitScale;
        
        if (ScaleModule)
            Particle.vWorldCurrentScale = ((Module[0].EndScale - Module[0].StartScale) * Particle.NormalizedAge + Module[0].StartScale) * Particle.vWorldInitScale;
        
        // Drag Module (�ӵ� ����)
        if (DragModule)
        {
            if (Particle.NormalizedAge < Module[0].DestNormalizedAge)
            {
                // ����
                // (��ǥ �ӷ� - ���� �ӷ�) / (��ǥ N.A - ���� N.A)
                float Gradient = (Module[0].LimitSpeed - length(Particle.vVelocity)) / (Module[0].DestNormalizedAge - Particle.NormalizedAge);
            
                // Normalized Age Delta Time (DT �� ���� ����ȭ�� ���� ���� ����)
                float NADT = DeltaTime / Particle.Life;
            
                float NewSpeed = length(Particle.vVelocity) + (Gradient * NADT);
                Particle.vVelocity = normalize(Particle.vVelocity) * NewSpeed;
            
            }
            else
            {
                if (length(Particle.vVelocity) > Module[0].LimitSpeed)
                {
                    Particle.vVelocity = normalize(Particle.vVelocity) * Module[0].LimitSpeed;
                }
            }
        }
                  
        if (RenderModule)
        {
            Particle.vColor.rgb = (Module[0].EndColor - Module[0].vSpawnColor.rgb) * Particle.NormalizedAge + Module[0].vSpawnColor.rgb;
            
            if(Module[0].FadeOut)
            {
                // ������ FadeOutStartRatio �κ� ���� Alpha ���� 1 ���� 0 ���� �پ������
                // ��, ������ �κ� �������� Alpha ���� 1�� �����Ǿ�� �� (saturate : 0 ���� 1 �̻����� ���� �� ����� ��.)
                float fRatio = saturate(1.f - ((Particle.NormalizedAge - Module[0].FadeOutStartRatio) / (1.f - Module[0].FadeOutStartRatio)));
                
                Particle.vColor.a = fRatio;
            }
        }
        
        Particle.Age += DeltaTime;
        //Particle.vWorldScale -= (float3) DeltaTime * 5.f;
        
        Particle.NormalizedAge = Particle.Age / Particle.Life;
        
        if(Particle.Life <= Particle.Age)
        {
            Particle.Active = 0;
        }
    }
}

#endif