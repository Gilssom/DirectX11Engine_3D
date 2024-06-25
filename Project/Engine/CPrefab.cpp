#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"
#include "CTransform.h"

PREFAB_SAVE_FUNC CPrefab::SAVE_FUNC = nullptr;
PREFAB_LOAD_FUNC CPrefab::LOAD_FUNC = nullptr;

CPrefab::CPrefab(bool bEngine)
	: CAsset(ASSET_TYPE::PREFAB, bEngine)
	, m_ProtoObject(nullptr)
{

}

CPrefab::CPrefab(CGameObject* proto)
	: CAsset(ASSET_TYPE::PREFAB, false)
	, m_ProtoObject(proto)
{
	m_ProtoObject->Transform()->SetRelativePos(0.f, 0.f, 0.f);
}

CPrefab::CPrefab(const CPrefab& other)
	: CAsset(other)
	, m_ProtoObject(nullptr)
{
	// Prefab ³¢¸®ÀÇ º¹Á¦
	m_ProtoObject = other.m_ProtoObject->Clone();
	m_ProtoObject->Transform()->SetRelativePos(0.f, 0.f, 0.f);
}

CPrefab::~CPrefab()
{
	if(m_ProtoObject != nullptr)
		delete m_ProtoObject;
}

CGameObject* CPrefab::Instantiate()
{
	return m_ProtoObject->Clone();
}

int CPrefab::Save(const wstring& FilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, FilePath.c_str(), L"wb");

	SAVE_FUNC(m_ProtoObject, pFile);

	fclose(pFile);

	return S_OK;
}

int CPrefab::Load(const wstring& FilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, FilePath.c_str(), L"rb");

	if (pFile == nullptr)
		return E_FAIL;
	//assert(pFile);

	m_ProtoObject = LOAD_FUNC(pFile);

	fclose(pFile);

	return S_OK;
}