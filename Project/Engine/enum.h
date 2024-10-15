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
	BOUNDINGBOX,

	MESHRENDER,
	SKYBOX,
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
	MESHDATA,
	MATERIAL,
	TEXTURE,
	SOUND,
	GRAPHICS_SHADER,
	COMPUTE_SHADER,

	END,
};

extern const char* ASSET_TYPE_STRING[(UINT)ASSET_TYPE::END];


// ��� ���� ������ ���� ��������
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
	CULL_BACK,		// �ݽð� �������� �׷����� ���� �����Ѵ�.
	CULL_FRONT,		// �ð�	  �������� �׷����� ���� �����Ѵ�.
	CULL_NONE,
	WIRE_FRAME,

	END,
};

// Depth Stencil State
enum class DS_TYPE
{
	LESS,		// ���� ���
	LESS_EQUAL, // �۰ų� ���� ���

	GREATER,	// �� ū ���

	NO_WRITE,	// ���� �׽�Ʈ�� ������, ������ ���̴� ��� X
	NO_TEST,	// ���� �׽�Ʈ ��� ���� �ٷ� ���
	NO_TEST_NO_WRITE, // ���� �׽�Ʈ�� ��� ����, �׸����� ���� ��

	// Volume Mesh
	BACKFACE_CHECK,		// �޸麸�� �տ� �ִ� �κ��� ã�´�.
	FRONTFACE_CHECK,	// �ո麸�� �ڿ� �ִ� �κ��� ã�´�.
	STENCIL_CHECK,		// ������ Stencil ���� �ش��ϴ� �κи� �����Ѵ�.

	END,
};

// Blend State
enum class BS_TYPE
{							//  TEX     BACK
	DEFAULT,				// Src(1), Dest(0)
	ALPHA_BLEND,			// Src(A), Dest(1-A)
	ALPHA_BLEND_COVERAGE,
	ONE_ONE,				// Src(1), Dest(1) : ������ ����� �� ��Ȳ, Light ȥ�� ��� ����� ��

	DECAL_BLEND,			// 0 -> AlpahBlend, 1 -> OneOne Blend

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
	DIRECTIONAL,	// ���⼺		- �¾籤(���� ���� = Global Light)
	POINT,			// ������		- ����, ȶ��
	SPOT,			// ��������Ʈ	- ����
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
	CONE,
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
	DOMAIN_DEFERRED,	// ���۵� ����
	DOMAIN_DECAL,		// ���۵� ��Į
	DOMAIN_LIGHTING,	// ���۵� ������(����)
	DOMAIN_SHADOWMAP,	// ������� ����

	DOMAIN_OPAQUE,		// ������
	DOMAIN_MASKED,		// �Ϻ� ����
	DOMAIN_TRANSPARENT,	// ������

	DOMAIN_PARTICLE,	// ��ƼŬ
	DOMAIN_POSTPROCESS, // ��ó��
};

enum class PARTICLE_MODULE
{
	SPAWN,			// �������� Spawn
	SPAWN_BURST,	// �������� Spawn
	ADD_VELOCITY,	// ���ӵ� �� ��
	SCALE,			// ũ��
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

enum class MRT_TYPE
{
	SWAPCHAIN,
	
	DEFERRED,
	DECAL,
	LIGHT,
	BRIGHTPASS,

	END,
};
