#pragma once
#include "Singletone.h"

#include "CPathManager.h"
#include "assets.h"

class CAssetManager : public CSingleton<CAssetManager>
{
    SINGLE(CAssetManager)

private:
    // Asset 관리 > 8종류의 키값으로 관리
    map<wstring, Ptr<CAsset>>   m_mapAsset[(UINT)ASSET_TYPE::END];
    bool                        m_AssetChanged;     // Asset 에 변경 사항이 생길 경우

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

    // Asset 불러오기
    template<typename T>
    Ptr<T> Load(const wstring& strKey, const wstring& strRelativePath);

    // Asset 검색
    template<typename T>
    Ptr<T> FindAsset(const wstring& strKey); // 무조건 스마트 포인터로 받을 수 있게

    // Asset 등록
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

// 변수 템플릿
//template<typename T1, typename T2>
//constexpr bool mybool = false;
// 부분 특수화
//template<typename T1>
//constexpr bool mybool<T1, T1> = true; // 두 typename 이 동일할 때 걸어줄 수 있는 특수화
// const 는 조금 더 넓은 개념의 상수화 ( 런타임 )
// constexpr 는 조금 더 좁은 엄격한 개념의 상수화 ( 컴파일 )
// = std::is_same_v ( 동일한 기능 )

template<typename T>
ASSET_TYPE GetAssetType()
{
    // type_info 는 런타임 도중 실행이 됨
    //const type_info& info = typeid(T);

    // constexpr =>> 컴파일 시 if 문을 확인함 > false 를 반환하면 if 문을 컴파일 시 완전히 없애버림
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
        strFullPath += strRelativePath;

        pAsset = new T;

        if (FAILED(pAsset->Load(strFullPath)))
        {
            MessageBox(nullptr, strFullPath.c_str(), L"에셋 로딩 실패", MB_OK);
            return nullptr;
        }

        // 본인 상대경로 세팅
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
    // Mesh 쪽 map 에 Texture 나 다른 T 가 들어갈 수 있는 상황이 발생할 수도 있다.
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

    assert(pFindAsset.Get() == nullptr); // 중복 확인

    ASSET_TYPE type = GetAssetType<T>();

    m_mapAsset[(UINT)type].insert(make_pair(strKey, pAsset.Get()));
    pAsset->m_Key = strKey;

    m_AssetChanged = true;
}