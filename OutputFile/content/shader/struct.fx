#ifndef _STRUCT
#define _STRUCT

// 광원 관련 구조체
struct tLight
{
    float4 vDiffuse;        // 난반사 (빛의 색상)
    float4 vAmbient;        // 환경광 (빛으로 발생하는 최소한의 밝기)
    float4 vMaxSpecular;    // 반사광 (빛으로 발생하는 반사광의 최대 밝기)
};

// 광원 정보
struct tLightInfo
{
    tLight  Light;          // 광원 색상 정보

    uint    LightType;      // 0 : Directional Light | 1 : Point Light | 2 : Spot Light
    float3  WorldDir;      // World Space 에서 광원의 방향
    float3  WorldPos;     // World Space 에서 광원의 위치
    float   Range;          // Spot Light 또는 Point Light 인 경우 광원의 영향 범위
    float   Angle;          // Spot Light 인 경우 광원의 각도
    float3  Padding;       // 16 Byte 맞추기 위한 용도
};

// 파티클 정보
struct tParticle
{
    float4  vColor;
    float3  vLocalPos;
    float3  vWorldPos;
    float3  vWorldInitScale;
    float3  vWorldCurrentScale;
    float3  vWorldRatation;
    
    float3  vForce;
    float3  vVelocity;
    
    float   NoiseForceAccTime;  // Noise Force Term 에 도달하는 누적 시간
    float3  NoiseForceDir;      // 적용된 Noise Force 의 방향
    
    float   Mass;
    float   Life;
    float   Age;
    float   NormalizedAge; // 현재 수명의 비율
    int     Active;
    
    float2  padding;
};

struct tSpawnCount
{
    int spawnCount;
    uint3 padding;
};

struct tParticleModule
{
    // ==============
    //  Spawn Module
    // ==============
    uint    vSpawnRate;                // 초당 파티클 생성 개수
    float4  vSpawnColor;             // 생성 시점 색상
    float4  vSpawnMinScale;          // 생성 시, 최소 크기
    float4  vSpawnMaxScale;          // 생성 시, 최대 크기

    float   MinLife;                  // 파티클 최소 수명
    float   MaxLife;                  // 파티클 최대 수명

    // Spawn Area (No Module)
    uint    SpawnShape;             // 0 : Box,   1 : Sphere
    float3  SpawnShapeScale;        // SpawnShapeSphere.x == Radius

	// Don't Spawn Area
    uint    BlockSpawnShape;        // 0 : Box,   1 : Sphere
    float3  BlockSpawnShapeScale;   // SpawnShapeSphere.x == Radius
    
    uint    SpaceType;              // 0 : LocalSpace,  1 : WorldSpace
    
    
    // ==============
    //  Spawn Burst
    // ==============
    uint    SpawnBurstCount;        // 한번에 생성할 파티클 개수
    uint    SpawnBurstRepeat;       // 재생성 할지 안할지
    float   SpawnBurstRepeatTime;   // 재생성 시간 간격
    
    
    // ==============
    //  Add Velocity
    // ==============
    uint    AddVelocityType;        // (방향) 0 : Random, 1 : From Center(중심에서 밖), 2 : To Center(밖에서 중심), 3 : Fixed(고정 방향)
    float3  AddVelocityFixedDir;
    float   AddMinSpeed;            // (최소 속도)
    float   AddMaxSpeed;            // (최대 속도)
    
    
    // ==============
    //  Scale
    // ==============
    float   StartScale;
    float   EndScale;
    
    // ==============
    //  Drag Module
    // ==============
    float   DestNormalizedAge;
    float   LimitSpeed;
   
    
    // ====================
    //  Noise Force Module
    // ====================
    float   NoiseForceTerm;         // Noise Force 적용 간격
    float   NoiseForceScale;        // Noise Force 크기


    // ====================
    //  Noise Force Module
    // ====================
    float3  EndColor;               // 최종 변환 색상
    uint    FadeOut;                // 0 : Off,  1 : Normalized Age
    float   FadeOutStartRatio;      // Fade Out 효과가 시작되는 Normalized Age 지점
    uint    VelocityAlignment;      // 속도 정렬 0 : Off, 1 : On
    
	// Module On / Off
    int     Module[7];
};

#endif