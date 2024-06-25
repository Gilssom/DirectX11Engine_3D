#pragma once

// 자료형 크기를 1Byte 기준으로 정렬을 하게 해준다.
//#pragma pack(1)

// 3차원 공간에서 좌표를 나타낼 수 있는 " Vertex 정점 " 정보들
struct Vtx
{
	// 원래라면 32Byte 구조체인데
	// DX 시스템으로 인해 28Byte 구조체로 정렬된다.
	Vec3 vPos;		// 좌표
	Vec4 vColor;	// 색상 및 투명도
	Vec2 vUv;		// UV 좌표
};

// Debug Render 요청 정보
struct tDebugShapeInfo
{
	DEBUG_SHAPE		Shape;
	Vec3			Position;
	Vec3			Scale;
	Vec3			Rotation;
	Matrix			matWorld; // 모든 위치 회전 크기를 가진 월드 행렬
	Vec4			Color;
	float			Duration; // 유지 시간
	float			Age;	  // 존재 시간
};

// Task 관련 구조체
struct tTask
{
	TASK_TYPE	Type;

	DWORD_PTR	dwParam_0;
	DWORD_PTR	dwParam_1;
	DWORD_PTR	dwParam_2;
};

// 광원 관련 구조체
struct tLight
{
	Vec4	vDiffuse;		// 난반사 (빛의 색상)
	Vec4	vAmbient;		// 환경광 (빛으로 발생하는 최소한의 밝기)
	Vec4	vMaxSpecular;	// 반사광 (빛으로 발생하는 반사광의 최대 밝기)
};

// 광원 정보
struct tLightInfo
{
	tLight	Light;

	UINT	LightType;	// 0 : Directional Light | 1 : Point Light | 2 : Spot Light
	Vec3	WorldDir;	// World Space 에서 광원의 방향
	Vec3	WorldPos;	// World Space 에서 광원의 위치
	float	Range;		// Spot Light 또는 Point Light 인 경우 광원의 영향 범위
	float	Angle;		// Spot Light 인 경우 광원의 각도

	Vec3	vPadding;	// 16 Byte 맞추기 위한 용도
};

// 파티클 (입자 한개 당 가지고 있어야 할 정보들)
struct tParticle
{
	Vec4	vColor;
	Vec3	vLocalPos;
	Vec3	vWorldPos;
	Vec3	vWorldInitScale;		// 최초 크기
	Vec3	vWorldCurrentScale;		// 현재 크기
	Vec3	vWorldRatation;

	Vec3	vForce;					// Particle 에 누적된 힘
	Vec3	vVelocity;

	float	NoiseForceAccTime;		// Noise Force Term 에 도달하는 누적 시간
	Vec3	NoiseForceDir;			// 적용된 Noise Force 의 방향

	float	Mass;					// 질량 추가
	float	Life;
	float	Age;
	float	NormalizedAge;			// 현재 수명의 비율
	int		Active;

	Vec2	padding;
};

// 파티클 Spawn Count
struct tSpawnCount
{
	int		SpawnCount;
	UINT	padding[3];
};

// 파티클 모듈
struct tParticleModule
{
	// Spawn Module (Base Info)
	UINT	vSpawnRate;				// 초당 파티클 생성 개수 (Spawn Per Second)
	Vec4	vSpawnColor;			// 생성 시점 색상
	Vec4	vSpawnMinScale;			// 생성 시, 최소 크기
	Vec4	vSpawnMaxScale;			// 생성 시, 최대 크기

	float	MinLife;				// 파티클 최소 수명
	float	MaxLife;				// 파티클 최대 수명

	// Spawn Area (No Module)
	UINT	SpawnShape;				// 0 : Box,   1 : Sphere
	Vec3	SpawnShapeScale;		// SpawnShapeSphere.x == Radius

	// Don't Spawn Area
	UINT	BlockSpawnShape;		// 0 : Box,   1 : Sphere
	Vec3	BlockSpawnShapeScale;	// SpawnShapeSphere.x == Radius

	UINT	SpaceType;				// 0 : LocalSpace,  1 : WorldSpace

	// Spawn Burst
	UINT	SpawnBurstCount;		// 한번에 생성할 파티클 개수
	UINT	SpawnBurstRepeat;		// 재생성 할지 안할지
	float	SpawnBurstRepeatTime;	// 재생성 시간 간격


	// Add Velocity
	UINT	AddVelocityType;		// (방향) 0 : Random, 1 : From Center(중심에서 밖), 2 : To Center(밖에서 중심), 3 : Fixed(고정 방향)
	Vec3	AddVelocityFixedDir;
	float	AddMinSpeed;			// (속도)
	float	AddMaxSpeed;			// (속도)


	// Scale Module
	float	StartScale;				// 최초 크기 비율
	float	EndScale;				// 최종 크기 비율


	// Drag Module
	float	DestNormalizedAge;
	float	LimitSpeed;


	// Noise Force Module
	float	NoiseForceTerm;			// Noise Force 적용 간격
	float	NoiseForceScale;		// Noise Force 크기


	// Render Module
	Vec3	EndColor;				// 최종 변환 색상
	UINT	FadeOut;				// 0 : Off,  1 : Normalized Age
	float	FadeOutStartRatio;		// Fade Out 효과가 시작되는 Normalized Age 지점 
	UINT	VelocityAlignment;		// 속도 정렬 0 : Off, 1 : On


	// Module On / Off
	int		Module[(UINT)PARTICLE_MODULE::END];
};


// ====================
// 상수버퍼 관련 구조체
// ====================
struct tTransform
{
	// 상수 버퍼 변수는 16 Byte 로 메모리 크기를 지정해줘야 한다.
	// 행렬 형태 사용
	Matrix matWorld;
	Matrix matView; // 카메라 전용
	Matrix matProj; // 투영 행렬

	Matrix matWV;
	Matrix matWVP;
};

// 전역 변수 - extern.cpp 에서 선언해줘서 사용 가능하게
extern tTransform g_Trans;

// 상수버퍼 Material 관련 구조체
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
	Vec2	vResolution;		// 렌더링 해상도
	float	DeltaTime;			// DT (레벨 상태에 영향을 받음)
	float	Time;				// 게임 시작한 후 경과한 시간 (레벨 상태에 영향을 받음)

	float	DeltaTimeEngine;	// (레벨 상태에 영향을 받지 않음)
	float	TimeEngine;			// (레벨 상태에 영향을 받지 않음)

	int		Light2DCount;		// 2D 광원 개수
	int		Light3DCount;		// 3D 광원 개수
};

extern tGlobalData g_GlobalData;