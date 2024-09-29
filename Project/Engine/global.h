#pragma once


#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXTex/DirectXTex.h>

#include "SimpleMath.h"

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug")
#else
#pragma comment(lib, "DirectXTex/DirectXTex")
#endif

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace DirectX::SimpleMath;

#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <map>
#include <string>
#include <typeinfo>

using std::vector;
using std::list;
using std::map;
using std::make_pair;
using std::string;
using std::wstring;


// File System
#include <filesystem>
using namespace std::filesystem;


// Fbx Loader
#include <FBXLoader\fbxsdk.h>

#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/release/libfbxsdk-md")
#endif


#include "define.h"
#include "enum.h"
#include "struct.h"
#include "ptr.h"
#include "func.h"
#include "Singletone.h"