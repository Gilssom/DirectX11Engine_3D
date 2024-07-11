#pragma once

// �ڷ��� ũ�⸦ 1Byte �������� ������ �ϰ� ���ش�.
//#pragma pack(1)

// 3���� �������� ��ǥ�� ��Ÿ�� �� �ִ� " Vertex ���� " ������
struct Vtx
{
	// ������� 32Byte ����ü�ε�
	// DX �ý������� ���� 28Byte ����ü�� ���ĵȴ�.
	Vec3 vPos;		// ��ǥ
	Vec4 vColor;	// ���� �� ����
	Vec2 vUv;		// UV ��ǥ


	// ť�긦 �����ϱ� ���� ������ ������ �� 24���� �ʿ��ϴ�.
	// ť���� �������� �� 3��� ������ �ϴ� ���̴�.
	// �׷��� �������� �� 3���� ������ ������ ������ �־�� �ϱ� ������ �� 24���� �ʿ��� ��.
	// �� ���� �����ϴ� �� �ȿ� �ִ� �ȼ��鿡�� ������ �־�� �Ѵ�.

	Vec3 vTangent;		// ���� ����
	Vec3 vNormal;		// ����	����
	Vec3 vBinormal;		// ������ ����
};

// Debug Render ��û ����
struct tDebugShapeInfo
{
	DEBUG_SHAPE		Shape;
	Vec3			Position;
	Vec3			Scale;
	Vec3			Rotation;
	Matrix			matWorld; // ��� ��ġ ȸ�� ũ�⸦ ���� ���� ���
	Vec4			Color;
	bool			DepthTest;
	float			Duration; // ���� �ð�
	float			Age;	  // ���� �ð�
};

// Task ���� ����ü
struct tTask
{
	TASK_TYPE	Type;

	DWORD_PTR	dwParam_0;
	DWORD_PTR	dwParam_1;
	DWORD_PTR	dwParam_2;
};

// ���� ���� ����ü
struct tLight
{
	Vec4	vDiffuse;		// ���ݻ� (���� ����)
	Vec4	vAmbient;		// ȯ�汤 (������ �߻��ϴ� �ּ����� ���)
	Vec4	vMaxSpecular;	// �ݻ籤 (������ �߻��ϴ� �ݻ籤�� �ִ� ���)
};

// ���� ����
struct tLightInfo
{
	tLight	Light;

	UINT	LightType;	// 0 : Directional Light | 1 : Point Light | 2 : Spot Light
	Vec3	WorldDir;	// World Space ���� ������ ����
	Vec3	WorldPos;	// World Space ���� ������ ��ġ
	float	Range;		// Spot Light �Ǵ� Point Light �� ��� ������ ���� ����
	float	Angle;		// Spot Light �� ��� ������ ����

	Vec3	vPadding;	// 16 Byte ���߱� ���� �뵵
};

// ��ƼŬ (���� �Ѱ� �� ������ �־�� �� ������)
struct tParticle
{
	Vec4	vColor;
	Vec3	vLocalPos;
	Vec3	vWorldPos;
	Vec3	vWorldInitScale;		// ���� ũ��
	Vec3	vWorldCurrentScale;		// ���� ũ��
	Vec3	vWorldRatation;

	Vec3	vForce;					// Particle �� ������ ��
	Vec3	vVelocity;

	float	NoiseForceAccTime;		// Noise Force Term �� �����ϴ� ���� �ð�
	Vec3	NoiseForceDir;			// ����� Noise Force �� ����

	float	Mass;					// ���� �߰�
	float	Life;
	float	Age;
	float	NormalizedAge;			// ���� ������ ����
	int		Active;

	Vec2	padding;
};

// ��ƼŬ Spawn Count
struct tSpawnCount
{
	int		SpawnCount;
	UINT	padding[3];
};

// ��ƼŬ ���
struct tParticleModule
{
	// Spawn Module (Base Info)
	UINT	vSpawnRate;				// �ʴ� ��ƼŬ ���� ���� (Spawn Per Second)
	Vec4	vSpawnColor;			// ���� ���� ����
	Vec4	vSpawnMinScale;			// ���� ��, �ּ� ũ��
	Vec4	vSpawnMaxScale;			// ���� ��, �ִ� ũ��

	float	MinLife;				// ��ƼŬ �ּ� ����
	float	MaxLife;				// ��ƼŬ �ִ� ����

	// Spawn Area (No Module)
	UINT	SpawnShape;				// 0 : Box,   1 : Sphere
	Vec3	SpawnShapeScale;		// SpawnShapeSphere.x == Radius

	// Don't Spawn Area
	UINT	BlockSpawnShape;		// 0 : Box,   1 : Sphere
	Vec3	BlockSpawnShapeScale;	// SpawnShapeSphere.x == Radius

	UINT	SpaceType;				// 0 : LocalSpace,  1 : WorldSpace

	// Spawn Burst
	UINT	SpawnBurstCount;		// �ѹ��� ������ ��ƼŬ ����
	UINT	SpawnBurstRepeat;		// ����� ���� ������
	float	SpawnBurstRepeatTime;	// ����� �ð� ����


	// Add Velocity
	UINT	AddVelocityType;		// (����) 0 : Random, 1 : From Center(�߽ɿ��� ��), 2 : To Center(�ۿ��� �߽�), 3 : Fixed(���� ����)
	Vec3	AddVelocityFixedDir;
	float	AddMinSpeed;			// (�ӵ�)
	float	AddMaxSpeed;			// (�ӵ�)


	// Scale Module
	float	StartScale;				// ���� ũ�� ����
	float	EndScale;				// ���� ũ�� ����


	// Drag Module
	float	DestNormalizedAge;
	float	LimitSpeed;


	// Noise Force Module
	float	NoiseForceTerm;			// Noise Force ���� ����
	float	NoiseForceScale;		// Noise Force ũ��


	// Render Module
	Vec3	EndColor;				// ���� ��ȯ ����
	UINT	FadeOut;				// 0 : Off,  1 : Normalized Age
	float	FadeOutStartRatio;		// Fade Out ȿ���� ���۵Ǵ� Normalized Age ���� 
	UINT	VelocityAlignment;		// �ӵ� ���� 0 : Off, 1 : On


	// Module On / Off
	int		Module[(UINT)PARTICLE_MODULE::END];
};


// ====================
// ������� ���� ����ü
// ====================
struct tTransform
{
	// ��� ���� ������ 16 Byte �� �޸� ũ�⸦ ��������� �Ѵ�.
	// ��� ���� ���
	Matrix matWorld;
	Matrix matWorldInv;
	Matrix matView; // ī�޶� ����
	Matrix matViewInv;
	Matrix matProj; // ���� ���
	Matrix matProjInv;

	Matrix matWV;
	Matrix matWVP;
};

// ���� ���� - extern.cpp ���� �������༭ ��� �����ϰ�
extern tTransform g_Trans;

// ������� Material ���� ����ü
struct tMtrlConst
{
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	matArr[4];
	UINT	btex[16];
};

struct tAnim2DInfo
{
	Vec2	vLeftTop;
	Vec2	vSliceSize;

	Vec2	vOffset;
	Vec2	vBackGround;

	int		UseAnim2D;
	int		padding[3];
};

struct tGlobalData
{
	Vec2	vResolution;		// ������ �ػ�
	float	DeltaTime;			// DT (���� ���¿� ������ ����)
	float	Time;				// ���� ������ �� ����� �ð� (���� ���¿� ������ ����)

	float	DeltaTimeEngine;	// (���� ���¿� ������ ���� ����)
	float	TimeEngine;			// (���� ���¿� ������ ���� ����)

	int		Light2DCount;		// 2D ���� ����
	int		Light3DCount;		// 3D ���� ����
};

extern tGlobalData g_GlobalData;