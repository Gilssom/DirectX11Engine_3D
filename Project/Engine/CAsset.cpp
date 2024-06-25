#include "pch.h"
#include "CAsset.h"

CAsset::CAsset(ASSET_TYPE type, bool bEngine)
	: m_Type(type)
	, m_RefCount(0)
	, m_bEngine(bEngine)
{

}

CAsset::CAsset(const CAsset& _Origin)
	: CEntity(_Origin) // 부모 복사 생성자도 명시적으로 호출
	, m_Key(_Origin.m_Key)
	, m_RelativePath(_Origin.m_RelativePath)
	, m_Type(_Origin.m_Type)
	, m_RefCount(0)
	, m_bEngine(false)
{

}

CAsset::~CAsset()
{

}
