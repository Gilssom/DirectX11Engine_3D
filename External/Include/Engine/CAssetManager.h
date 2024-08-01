#pragma once
#include "Singletone.h"

#include "CPathManager.h"
#include "assets.h"

class CAssetManager : public CSingleton<CAssetManager>
{
    SINGLE(CAssetManager)

private:
    // Asset ���� > 8������ Ű������ ����
    map<wstring, Ptr<CAsset>>   m_mapAsset[(UINT)ASSET_TYPE::END];
    bool                        m_AssetChanged;     // Asset �� ���� ������ ���� ���

public:
    void Init();
    void Tick() { m_AssetChanged = false; }
    void GetAssetNames(ASSET_TYPE type, _Out_ vector<string>& vecNames);
    const map<wstring, Ptr<CAsset>>& GetAssets(ASSET_TYPE type) { return m_mapAsset[(UINT)type]; }

    bool IsAssetChanged() { return m_AssetChanged; }

    // bindFlag : D3D11_BIND_FLAG
    Ptr<CTexture> CreateTexture(const wstring& strKey, UINT width, UINT height
                            , DXGI_FORMAT pixelFormat, UINT bindFlag, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

    Ptr<CTexture> CreateTexture(const wstring& strKey, ComPtr<ID3D11Texture2D> tex2D);

    Ptr<CTexture> CreateTexture(const wstring& strKey, const vector<Ptr<CTexture>>& vecTex);

    // Asset �ҷ�����
    template<typename T>
    Ptr<T> Load(const wstring& strKey, const wstring& strRelativePath = L"");

    // Asset �˻�
    template<typename T>
    Ptr<T> FindAsset(const wstring& strKey); // ������ ����Ʈ �����ͷ� ���� �� �ְ�

    // Asset ���
    template<typename T>
    void AddAsset(const wstring& strKey, Ptr<T> pAsset);

private:
    void DeleteAsset(ASSET_TYPE type, const wstring& strKey);

private:
    void CreateDefaultMesh();
    void CreateDefaultTexture();
    void CreateDefaultMaterial();
    void CreateDefaultGraphicShader();
    void CreateDefaultComputeShader();

    friend class CTaskManager;
};

// ���� ���ø�
//template<typename T1, typename T2>
//constexpr bool mybool = false;
// �κ� Ư��ȭ
//template<typename T1>
//constexpr bool mybool<T1, T1> = true; // �� typename �� ������ �� �ɾ��� �� �ִ� Ư��ȭ
// const �� ���� �� ���� ������ ���ȭ ( ��Ÿ�� )
// constexpr �� ���� �� ���� ������ ������ ���ȭ ( ������ )
// = std::is_same_v ( ������ ��� )

template<typename T>
ASSET_TYPE GetAssetType()
{
    // type_info �� ��Ÿ�� ���� ������ ��
    //const type_info& info = typeid(T);

    // constexpr =>> ������ �� if ���� Ȯ���� > false �� ��ȯ�ϸ� if ���� ������ �� ������ ���ֹ���
    if constexpr (std::is_same_v<T, CMesh>)
    {
        return ASSET_TYPE::MESH;
    }
    if constexpr (std::is_same_v<T, CGraphicShader>)
    {
        return ASSET_TYPE::GRAPHICS_SHADER;
    }
    if constexpr (std::is_same_v<T, CComputeShader>)
    {
        return ASSET_TYPE::COMPUTE_SHADER;
    }
    if constexpr (std::is_same_v<T, CTexture>)
    {
        return ASSET_TYPE::TEXTURE;
    }
    if constexpr (std::is_same_v<T, CMaterial>)
    {
        return ASSET_TYPE::MATERIAL;
    }
    if constexpr (std::is_same_v<T, CPrefab>)
    {
        return ASSET_TYPE::PREFAB;
    }
    if constexpr (std::is_same_v<T, CSound>)
    {
        return ASSET_TYPE ::SOUND;
    }
}

template<typename T>
inline Ptr<T> CAssetManager::Load(const wstring& strKey, const wstring& strRelativePath)
{
    Ptr<CAsset> pAsset = FindAsset<T>(strKey).Get();
    
    if (pAsset.Get() != nullptr)
    {
        return (T*)pAsset.Get();
    }

    if constexpr (std::is_same_v<T, CComputeShader> || std::is_same_v<T, CGraphicShader>)
    {
        return nullptr;
    }
    else
    {
        wstring strFullPath = CPathManager::GetInst()->GetContentPath();

        if(!strRelativePath.empty())
            strFullPath += strRelativePath;
        else
            strFullPath += strKey;

        pAsset = new T;

        if (FAILED(pAsset->Load(strFullPath)))
        {
            MessageBox(nullptr, strFullPath.c_str(), L"���� �ε� ����", MB_OK);
            return nullptr;
        }

        // ���� ����� ����
        pAsset->m_RelativePath = strRelativePath;

        AddAsset<T>(strKey, (T*)pAsset.Get());

        m_AssetChanged = true;

        return (T*)pAsset.Get();
    }
}

template<typename T>
inline Ptr<T> CAssetManager::FindAsset(const wstring& strKey)
{
    ASSET_TYPE type = GetAssetType<T>();

    map<wstring, Ptr<CAsset>>::iterator iter = m_mapAsset[(UINT)type].find(strKey);

    if (iter == m_mapAsset[(UINT)type].end())
    {
        return nullptr;
    }

#ifdef _DEBUG
    // Mesh �� map �� Texture �� �ٸ� T �� �� �� �ִ� ��Ȳ�� �߻��� ���� �ִ�.
    T* pAsset = dynamic_cast<T*> (iter->second.Get());
    return pAsset;
#else
    return (T*)iter->second.Get();
#endif
}

template<typename T>
inline void CAssetManager::AddAsset(const wstring& strKey, Ptr<T> pAsset)
{
    Ptr<T> pFindAsset = FindAsset<T>(strKey);

    assert(pFindAsset.Get() == nullptr); // �ߺ� Ȯ��

    ASSET_TYPE type = GetAssetType<T>();

    m_mapAsset[(UINT)type].insert(make_pair(strKey, pAsset.Get()));
    pAsset->m_Key = strKey;

    m_AssetChanged = true;
}