#pragma once

enum class COMPONENT_TYPE
{
	TRANSFORM,
	CAMERA,
	COLLIDER2D,
	COLLIDER3D,
	ANIMATOR2D,
	ANIMATOR3D,
	LIGHT2D,
	LIGHT3D,

	MESHRENDER,
	DECAL,
	PARTICLESYSTEM,
	TILEMAP,
	LANDSCAPE,

	END,

	SCRIPT,
};

extern const char* COMPONENT_TYPE_STRING[(UINT)COMPONENT_TYPE::END];


enum class ASSET_TYPE
{
	PREFAB,
	MESH,
	MESH_DATA,
	MATERIAL,
	TEXTURE,
	SOUND,
	GRAPHICS_SHADER,
	COMPUTE_SHADER,

	END,
};

extern const char* ASSET_TYPE_STRING[(UINT)ASSET_TYPE::END];


// 상수 버퍼 갯수는 거의 고정적임
enum class CB_TYPE
{
	TRANSFORM,
	MATERIAL,
	ANIMATION,
	GLOBAL,

	END,
};

// Rasterizer State
enum class RS_TYPE
{
	CULL_BACK,		// 반시계 방향으로 그려지는 것을 제외한다.
	CULL_FRONT,		// 시계	  방향으로 그려지는 것을 제외한다.
	CULL_NONE,
	WIRE_FRAME,

	END,
};

// Depth Stencil State
enum class DS_TYPE
{
	LESS,		// 작은 경우
	LESS_EQUAL, // 작거나 같은 경우

	GREATER,	// 더 큰 경우

	NO_WRITE,	// 깊이 테스트는 받지만, 본인의 깊이는 기록 X
	NO_TEST,	// 깊이 테스트 상관 없이 바로 통과
	NO_TEST_NO_WRITE, // 깊이 테스트도 상관 없고, 그리지도 않을 것

	END,
};

// Blend State
enum class BS_TYPE
{							//  TEX     BACK
	DEFAULT,				// Src(1), Dest(0)
	ALPHA_BLEND,			// Src(A), Dest(1-A)
	ALPHA_BLEND_COVERAGE,
	ONE_ONE,				// Src(1), Dest(1) : 검은색 배경을 뺄 상황, Light 혼합 기능 사용할 때

	END,
};

// Direction Type
enum class DIR_TYPE
{
	RIGHT,
	UP,
	FRONT,
};

extern Vec3	XAxis;
extern Vec3	YAxis;
extern Vec3	ZAxis;

// Light Type
enum class LIGHT_TYPE
{
	DIRECTIONAL,	// 방향성		- 태양광(전역 광원 = Global Light)
	POINT,			// 점광원		- 전구, 횃불
	SPOT,			// 스폿라이트	- 조명
};

enum SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
	MAT_2,
	MAT_3,
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEX_CUBE_0,
	TEX_CUBE_1,
	TEX_CUBE_2,
	TEX_CUBE_3,

	TEX_ARR_0,
	TEX_ARR_1,
	TEX_ARR_2,
	TEX_ARR_3,

	END,
};

enum class DEBUG_SHAPE
{
	// 2D
	RECT,
	CIRCLE,
	LINE,

	// 3D
	CUBE,
	SPHERE,
};

enum class PROJ_TYPE
{
	ORTHOGRAPHIC,
	PERSPECTIVE,
};

enum class TASK_TYPE
{
	// Param_0 : LayerIndex
	// Param_1 : Object Adress
	SPAWN_OBJECT,

	// Param_0 : Obejct Adress
	DESTROY_OBJECT,

	// Param_0 : Collider2D Adress
	COLLIDER2D_SEMI_DEACTIVE,
	COLLIDER2D_DEACTIVE,

	// Param_0 : Level Adress
	// Param_1 : LEVEL_STATE
	CHANGE_LEVEL,

	// Param_0 : LEVEL_STATE
	CHANGE_LEVEL_STATE,

	// Param_0 : Asset Adress
	DELETE_ASSET,
};

enum class SHADER_DOMAIN
{
	DOMAIN_OPAQUE,		// 불투명
	DOMAIN_MASKED,		// 일부 투명
	DOMAIN_TRANSPARENT,	// 반투명

	DOMAIN_PARTICLE,	// 파티클
	DOMAIN_POSTPROCESS, // 후처리
};

enum class PARTICLE_MODULE
{
	SPAWN,			// 지속적인 Spawn
	SPAWN_BURST,	// 순간적인 Spawn
	ADD_VELOCITY,	// 가속도 및 힘
	SCALE,			// 크기
	DRAG,
	NOISE_FORCE,
	RENDER,

	END,
};

enum class LEVEL_STATE
{
	PLAY,
	STOP,
	PAUSE,
};