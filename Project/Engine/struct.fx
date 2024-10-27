#ifndef _STRUCT
#define _STRUCT

struct tMtrlData
{
    float4 vDiff;
    float4 vSpec;
    float4 vAmb;
    float4 vEmv;
};

// ���� ���� ����ü
struct tLight
{
    float4 vDiffuse;        // ���ݻ� (���� ����)
    float4 vAmbient;        // ȯ�汤 (������ �߻��ϴ� �ּ����� ���)
    float4 vMaxSpecular;    // �ݻ籤 (������ �߻��ϴ� �ݻ籤�� �ִ� ���)
};

// ���� ����
struct tLightInfo
{
    tLight  Light;          // ���� ���� ����

    uint    LightType;      // 0 : Directional Light | 1 : Point Light | 2 : Spot Light
    float3  WorldDir;      // World Space ���� ������ ����
    float3  WorldPos;     // World Space ���� ������ ��ġ
    float   Range;          // Spot Light �Ǵ� Point Light �� ��� ������ ���� ����
    float   Angle;          // Spot Light �� ��� ������ ����
    float3  Padding;       // 16 Byte ���߱� ���� �뵵
};

// ��ƼŬ ����
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
    
    float   NoiseForceAccTime;  // Noise Force Term �� �����ϴ� ���� �ð�
    float3  NoiseForceDir;      // ����� Noise Force �� ����
    
    float   Mass;
    float   Life;
    float   Age;
    float   NormalizedAge; // ���� ������ ����
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
    uint    vSpawnRate;                // �ʴ� ��ƼŬ ���� ����
    float4  vSpawnColor;             // ���� ���� ����
    float4  vSpawnMinScale;          // ���� ��, �ּ� ũ��
    float4  vSpawnMaxScale;          // ���� ��, �ִ� ũ��

    float   MinLife;                  // ��ƼŬ �ּ� ����
    float   MaxLife;                  // ��ƼŬ �ִ� ����

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
    uint    SpawnBurstCount;        // �ѹ��� ������ ��ƼŬ ����
    uint    SpawnBurstRepeat;       // ����� ���� ������
    float   SpawnBurstRepeatTime;   // ����� �ð� ����
    
    
    // ==============
    //  Add Velocity
    // ==============
    uint    AddVelocityType;        // (����) 0 : Random, 1 : From Center(�߽ɿ��� ��), 2 : To Center(�ۿ��� �߽�), 3 : Fixed(���� ����)
    float3  AddVelocityFixedDir;
    float   AddMinSpeed;            // (�ּ� �ӵ�)
    float   AddMaxSpeed;            // (�ִ� �ӵ�)
    
    
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
    float   NoiseForceTerm;         // Noise Force ���� ����
    float   NoiseForceScale;        // Noise Force ũ��


    // ====================
    //  Noise Force Module
    // ====================
    float3  EndColor;               // ���� ��ȯ ����
    uint    FadeOut;                // 0 : Off,  1 : Normalized Age
    float   FadeOutStartRatio;      // Fade Out ȿ���� ���۵Ǵ� Normalized Age ����
    uint    VelocityAlignment;      // �ӵ� ���� 0 : Off, 1 : On
    
	// Module On / Off
    int     Module[7];
};

struct tRaycastOut
{
    float2  Location;
    uint    Distance;
    int     Success;
};

struct tWeight
{
    float Weight[8];
};

struct tGrassWeight
{
    float GrassWeight;
    
    float fadding[7];
};

struct tSkinningInfo
{
    float3 vPos;
    float3 vTangent;
    float3 vBinormal;
    float3 vNormal;
};

#endif