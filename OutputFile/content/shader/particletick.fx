#ifndef _PARTICLE_TICK
#define _PARTICLE_TICK

#include "value.fx"
#include "func.fx"


// Tick 을 호출 받을 Particle 들이 Binding 될 곳
RWStructuredBuffer<tParticle>   ParticleBuffer   : register(u0);
RWStructuredBuffer<tSpawnCount> SpawnCountBuffer : register(u1);
Texture2D                       NoiseTex         : register(t20);
StructuredBuffer<tParticleModule> Module         : register(t21);


// Particle Buffer 의 개수
#define ParticleBufferSize  g_int_0

// Particle Object 의 위치
#define ParticleObjectPos   g_vec4_0

// Particle Tick Buffer 접근
#define Particle            ParticleBuffer[_ID.x]

// Spawn Count Buffer 접근
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

// 유연하게 개수에 대처하기 위해서 적당히 32개만
[numthreads(32, 1, 1)]
void CS_ParticleTick(int3 _ID : SV_DispatchThreadID)
{
    // Thread 간의 동기화가 필요함
    // 1. 병렬 처리 이기 때문에 Count 가 1씩 감소하는 것이 아니라 
    //   전체가 한번에 동작하기 때문에 먹히지가 않음
    // 2. 그리고 랜덤값도 먹히지가 않음 (동시에 병렬처리로 하기 때문에 같은 랜덤값을 가지게 됨)
    // 위의 기능을 제공하는 함수가 내부에 존재함
    
    // Thread ID == Particle Index
    // Thread ID 가 Particle 개수를 초과하면 return
    if (ParticleBufferSize <= _ID.x)
        return;
    
    // InterlockedExchange 도 문제점이 있다.
    // 하나 하나씩 값 비교하면서 실행하는건 맞는데
    // 모두가 1회차, 2회차 ... n회차를 다 함께 실행하는게 아닐 수도 있다.(n이 높을수록 문제가 생길 가능성 상승)
    // 그래서 나중에 실행되는 Interlocked Thread 가 값을 들고있는 값으로 변경시키는 현상이 발생
    
    // InterlockedCompareExchange
    // dest 값과 compare value 를 한번 더 비교를 해서 위와 같은 문제가 발생할 수 없게 방지
    // 평균적으로 n회차의 첫 시도하는 Thread 만 통과를 하게 된다. 
    // (값 변경이 이루어지니 뒤의 Compare 조건이 통과가 안됨)
    
    //비활성화된 파티클을 활성화 시켜야함
    if (Particle.Active == 0)
    {
        // Spawn Module
        if (SpawnModule || SpawnBurstModule)
        {
            // 이번에 활성화 가능한 파티클 수 체크
            int CurSpawnCount = SpawnCount;
        
            // 파티클 활성화 가능 숫자가 0보다 크면
            while (0 < CurSpawnCount)
            {
                // Spawn Count 에 들어있던 값을 받을 변수
                int OriginValue = 0;
            
                //InterlockedExchange(SpawnCount, SpawnCount - 1, OriginValue);
                // 교체를 시도할 때, SpawnCount 에 들어있는 값이 예상 값과 동일한 경우에만 값을 교체한다.
                InterlockedCompareExchange(SpawnCount, CurSpawnCount, SpawnCount - 1, OriginValue);
            
                // 교체한 값이 원래 예상하던 값과 일치하면 활성화 성공
                // 진입한 Thread 가 존재하면 다음 Thread 들은 아래 조건에 들어올 수 없음
                // 다음 활성화 시도 단계로 진입 대기
                if (OriginValue == CurSpawnCount)
                {
                    // Random 함수화
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
                        
                        // 금지 영역 설정 (Sphere) - Box 는 개인 구현
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
            
                // 진입하지 못한 Thread 들은 활성화 가능한 파티클 수를 재체크
                CurSpawnCount = SpawnCount;
            }
        }
    }
    else
    {
        // Global Data 를 Binding_CS 를 통해서 받아왔으니 활용
        
        Particle.vForce = float3(0.f, 0.f, 0.f);
        
        // Noise Force Module (Random Direction Force)
        if (NoiseForceModule)
        {
            // 일정 시간마다 Noise Force 의 방향을 Random 으로 구한다.
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

        // Particle 에 주어진 힘에 따른 가속도 계산
        float3 vAccel = Particle.vForce / Particle.Mass;
        
        // 가속도에 따른 속도의 실시간 변화
        Particle.vVelocity += vAccel * DeltaTime;
        
        
        // Velocity 에 따른 이동
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
            
        
        // Scale Module 에 따른 현재 크기 계산
        Particle.vWorldCurrentScale = Particle.vWorldInitScale;
        
        if (ScaleModule)
            Particle.vWorldCurrentScale = ((Module[0].EndScale - Module[0].StartScale) * Particle.NormalizedAge + Module[0].StartScale) * Particle.vWorldInitScale;
        
        // Drag Module (속도 감소)
        if (DragModule)
        {
            if (Particle.NormalizedAge < Module[0].DestNormalizedAge)
            {
                // 기울기
                // (목표 속력 - 현재 속력) / (목표 N.A - 현재 N.A)
                float Gradient = (Module[0].LimitSpeed - length(Particle.vVelocity)) / (Module[0].DestNormalizedAge - Particle.NormalizedAge);
            
                // Normalized Age Delta Time (DT 가 가진 정규화된 수명에 대한 비율)
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
                // 설정한 FadeOutStartRatio 부분 부터 Alpha 값이 1 부터 0 까지 줄어들어야함
                // 또, 지정된 부분 이전에는 Alpha 값이 1이 유지되어야 함 (saturate : 0 에서 1 이상으로 값이 못 벗어나게 함.)
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