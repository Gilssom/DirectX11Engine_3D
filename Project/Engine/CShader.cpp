#include "pch.h"
#include "CShader.h"

// Shader 는 무조건 Engine 에서 제작할 것
CShader::CShader(ASSET_TYPE type)
	: CAsset(type, true)
{

}

CShader::~CShader()
{

}
