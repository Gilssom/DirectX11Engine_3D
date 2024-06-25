#include "pch.h"

tTransform g_Trans = {};

tGlobalData g_GlobalData = {};

extern Vec3	XAxis(1.f, 0.f, 0.f);
extern Vec3	YAxis(0.f, 1.f, 0.f);
extern Vec3	ZAxis(0.f, 0.f, 1.f);


extern const char* COMPONENT_TYPE_STRING[(UINT)COMPONENT_TYPE::END]
=
{
	"Transform",
	"Camera",
	"Collider2D",
	"Collider3D",
	"Aniamtor2D",
	"Animator3D",
	"Light2D",
	"Light3D",
	"Mesh Renderer",
	"Decal",
	"Particle System",
	"Tile Map",
	"LandScape"
};


extern const char* ASSET_TYPE_STRING[(UINT)ASSET_TYPE::END]
=
{
	"Prefab",
	"Mesh",
	"Mesh Data",
	"Material",
	"Texture",
	"Sound",
	"Graphics Shader",
	"Compute Shader"
};