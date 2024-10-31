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


	// 큐브를 구성하기 위한 정점의 갯수는 총 24개가 필요하다.
	// 큐브의 꼭짓점은 총 3면과 교차를 하는 점이다.
	// 그래서 꼭짓점은 총 3개의 정점의 정보를 가지고 있어야 하기 때문에 총 24개가 필요한 것.
	// 각 점은 교차하는 면 안에 있는 픽셀들에게 정보를 주어야 한다.

	Vec3 vTangent;		// 접선 벡터
	Vec3 vNormal;		// 법선	벡터
	Vec3 vBinormal;		// 종법선 벡터


	// 3D Animation Bone Mesh 추가로 인한 요소
	Vec4 vWeights;		// Bone 가중치
	Vec4 vIndices;		// Bone 인덱스
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
	bool			DepthTest;
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

struct tRay
{
	Vec3	vStart;
	Vec3	vDir;
};

// =================
//   Animation 3D
// =================
struct tFrameTrans
{
	Vec4 vTranslate;	// 위치
	Vec4 vScale;		// 크기
	Vec4 qRot;			// 회전
};

struct tMTKeyFrame
{
	double	dTime;		// 키 프레임 시간
	int		iFrame;		// 키 프레임 인덱스
	Vec3	vTranslate;	// 위치
	Vec3	vScale;		// 크기
	Vec4	qRot;		// 회전
};

struct tMTBone
{
	wstring				strBoneName;	// 본 이름
	int					iDepth;			// 본 깊이
	int					iParentIndx;	// 부모 본의 인덱스
	Matrix				matOffset;		// Inverse 행렬( Skin 정점을 -> 기본상태로 되돌림)
	Matrix				matBone;		// 본 행렬
	vector<tMTKeyFrame>	vecKeyFrame;	// 키 프레임 목록
};

struct tMTAnimClip
{
	wstring			strAnimName;		// 애니메이션 이름
	int				iStartFrame;		// 애니메이션 시작 프레임
	int				iEndFrame;			// 애니메이션 종료 프레임
	int				iFrameLength;		// 애니메이션 총 프레임 수

	double			dStartTime;			// 애니메이션 시작 시간
	double			dEndTime;			// 애니메이션 종료 시간
	double			dTimeLength;		// 애니메이션 지속 시간
	float			fUpdateTime;		// 이거 안씀

	FbxTime::EMode	eMode;				// 애니메이션 시간 모드(프레임 or 시간)
};

struct AnimationClip
{
	string			animName;
	int				startFrame;
	int				endFrame;
	int				frameLength;
	bool			repeat;

	AnimationClip()
		: animName(""), startFrame(0), endFrame(0), frameLength(0), repeat(false) { }

	AnimationClip(string _name, int _startFrame, int _endFrame, bool _repeat)
		: animName(_name), startFrame(_startFrame), endFrame(_endFrame), repeat(_repeat)
	{
		frameLength = endFrame - startFrame + 1;
	}

	struct SaveEvent
	{
		int frame;
		string eventName;
	};
	vector<SaveEvent> haveEvent;
};

struct AnimationEvent
{
	int frame;
	std::function<void()> callback;
	string eventName;
};


// ============
//  Instancing
// ============
union uInstID
{
	struct 
	{
		UINT iMesh;
		WORD iMtrl;
		WORD iMtrlIdx;
	};

	ULONG64	LLID;
};

class CGameObject;
struct tInstObject
{
	CGameObject*	pObject;
	UINT			iMtrlIdx;
};

struct tInstancingData
{
	Matrix	matWorld;
	Matrix	matWV;
	Matrix	matWVP;
	int		iRowIdx;
};


// ====================
// 상수버퍼 관련 구조체
// ====================
struct tTransform
{
	// 상수 버퍼 변수는 16 Byte 로 메모리 크기를 지정해줘야 한다.
	// 행렬 형태 사용
	Matrix matWorld;
	Matrix matWorldInv;
	Matrix matView; // 카메라 전용
	Matrix matViewInv;
	Matrix matProj; // 투영 행렬
	Matrix matProjInv;

	Matrix matWV;
	Matrix matWVP;
};

// 전역 변수 - extern.cpp 에서 선언해줘서 사용 가능하게
extern tTransform g_Trans;

// Material 계수
struct tMtrlData
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmv;
};

// 상수버퍼 Material 관련 구조체
struct tMtrlConst
{
	tMtrlData	mtrl;

	int			iArr[4];
	float		fArr[4];
	Vec2		v2Arr[4];
	Vec4		v4Arr[4];
	Matrix		matArr[4];
	UINT		btex[14];

	// 3D Animation 정보
	int			arrAnimData[2];
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

struct tClipPlane
{
	Vec4 clipPlane;
};

extern tClipPlane g_ClipPlane;