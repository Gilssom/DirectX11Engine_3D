#include "pch.h"
#include "CAssetManager.h"

CAssetManager::CAssetManager()
	: m_AssetChanged(false)
{

}

CAssetManager::~CAssetManager()
{

}

Ptr<CTexture> CAssetManager::CreateTexture(const wstring& strKey, UINT width, UINT height
										, DXGI_FORMAT pixelFormat, UINT bindFlag, D3D11_USAGE usage)
{
	Ptr<CTexture> pTex = FindAsset<CTexture>(strKey);

	if (pTex != nullptr)
	{
		return pTex;
	}

	pTex = new CTexture;
	pTex->Create(width, height, pixelFormat, bindFlag, usage);
	pTex->m_Key = strKey;

	m_mapAsset[(UINT)ASSET_TYPE::TEXTURE].insert(make_pair(strKey, pTex.Get()));

	return pTex;
}

Ptr<CTexture> CAssetManager::CreateTexture(const wstring& strKey, ComPtr<ID3D11Texture2D> tex2D)
{
	Ptr<CTexture> pTex = FindAsset<CTexture>(strKey);

	if (pTex != nullptr)
	{
		return pTex;
	}

	pTex = new CTexture;
	pTex->Create(tex2D);
	pTex->m_Key = strKey;

	m_mapAsset[(UINT)ASSET_TYPE::TEXTURE].insert(make_pair(strKey, pTex.Get()));

	return pTex;
}

Ptr<CTexture> CAssetManager::CreateTexture(const wstring& strKey, const vector<Ptr<CTexture>>& vecTex)
{
	Ptr<CTexture> pTex = FindAsset<CTexture>(strKey);

	if (pTex != nullptr)
	{
		return pTex;
	}

	pTex = new CTexture;

	if (FAILED(pTex->CreateArrayTexture(vecTex)))
	{
		MessageBox(nullptr, L"Array Texture 생성 실패", L"Texture 생성 오류", MB_OK);
		return nullptr;
	}

	pTex->m_Key = strKey;

	m_mapAsset[(UINT)ASSET_TYPE::TEXTURE].insert(make_pair(strKey, pTex.Get()));

	return pTex;
}

Ptr<CMeshData> CAssetManager::LoadFBX(const wstring& strPath)
{
	wstring strFileName = path(strPath).stem();

	wstring strName = L"meshdata\\";
	strName += strFileName + L".mdat";

	Ptr<CMeshData> pMeshData = FindAsset<CMeshData>(strName);

	if (nullptr != pMeshData)
		return pMeshData;

	pMeshData = CMeshData::LoadFromFBX(strPath);
	pMeshData->SetKey(strName);
	pMeshData->SetRelativePath(strName);

	m_mapAsset[(UINT)ASSET_TYPE::MESHDATA].insert(make_pair(strName, pMeshData.Get()));

	// meshdata 를 실제파일로 저장
	pMeshData->Save(CPathManager::GetInst()->GetContentPath() + strName);

	return pMeshData;
}

void CAssetManager::GetAssetNames(ASSET_TYPE type, _Out_ vector<string>& vecNames)
{
	for (const auto& pair : m_mapAsset[(UINT)type])
	{
		vecNames.push_back(string(pair.first.begin(), pair.first.end()));
	}
}

void CAssetManager::DeleteAsset(ASSET_TYPE type, const wstring& strKey)
{
	map<wstring, Ptr<CAsset>>::iterator iter = m_mapAsset[(UINT)type].find(strKey);

	assert(iter != m_mapAsset[(UINT)type].end());

	m_mapAsset[(UINT)type].erase(iter);

	m_AssetChanged = true;
}