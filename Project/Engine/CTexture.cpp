#include "pch.h"
#include "CTexture.h"

#include "CDevice.h"

CTexture::CTexture(bool bEngine)
    : CAsset(ASSET_TYPE::TEXTURE, bEngine)
{
}

CTexture::~CTexture()
{
}

void CTexture::Binding(int registerNum)
{
    CONTEXT->PSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Binding_CS_SRV(int registerNum)
{
    CONTEXT->CSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Binding_CS_UAV(int registerNum)
{
    UINT i = -1;
    CONTEXT->CSSetUnorderedAccessViews(registerNum, 1, m_UAV.GetAddressOf(), &i);
}

void CTexture::Clear(int registerNum)
{
    ID3D11ShaderResourceView* pSRV = nullptr;
    CONTEXT->PSSetShaderResources(registerNum, 1, &pSRV);
}

void CTexture::Clear_CS_SRV(int registerNum)
{
    ID3D11ShaderResourceView* pSRV = nullptr;
    CONTEXT->CSSetShaderResources(registerNum, 1, &pSRV);
}

void CTexture::Clear_CS_UAV(int registerNum)
{
    UINT i = -1;
    ID3D11UnorderedAccessView* pUAV = nullptr;
    CONTEXT->CSSetUnorderedAccessViews(registerNum, 1, &pUAV, &i);
}

int CTexture::Create(UINT width, UINT height, DXGI_FORMAT pixelFormat
                    , UINT bindFlag, D3D11_USAGE usage)
{
    m_Desc.Format = pixelFormat;
    m_Desc.Width = width;
    m_Desc.Height = height;
    m_Desc.ArraySize = 1;

    m_Desc.Usage = usage;
    if (D3D11_USAGE_DYNAMIC == m_Desc.Usage)
    {
        m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }

    // Bind Flag 가 조합이 될 수도 있다. (내부 값 확인 필요) 각자의 비트로 설계되어 있음 (정수타입으로 받게 됨)
    // D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER ...
    m_Desc.BindFlags = bindFlag;
    m_Desc.MipLevels = 1;
    m_Desc.SampleDesc.Count = 1;
    m_Desc.SampleDesc.Quality = 0;

    if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, &m_Tex2D)))
    {
        return E_FAIL;
    }

    // 비트 연산자로 해당 Bing Flag Type 확인
    if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
    }
    // Depth Stencil Flag 는 단일 Flag 이여야 하기 때문에 else 처리 (다른 것과 조합이 불가능)
    else
    {
        if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
        {
            // Render Target View
            DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
        }

        if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            // Shader Resource View
            D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = 1;
            desc.Texture2D.MostDetailedMip = 0;

            DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &desc, m_SRV.GetAddressOf());
        }

        if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
        {
            // Unordered Access View
            D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

            DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &desc, m_UAV.GetAddressOf());
        }
    }

    return S_OK;
}

int CTexture::Create(ComPtr<ID3D11Texture2D> tex2D)
{
    // 이미 만들어진 Texture 정보 가져오기
    m_Tex2D = tex2D;
    m_Tex2D->GetDesc(&m_Desc);

    // 비트 연산자로 해당 Bing Flag Type 확인
    if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
    }
    // Depth Stencil Flag 는 단일 Flag 이여야 하기 때문에 else 처리 (다른 것과 조합이 불가능)
    else
    {
        if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
        {
            // Render Target View
            DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
        }

        if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            // Shader Resource View
            D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = 1;
            desc.Texture2D.MostDetailedMip = 0;

            DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &desc, m_SRV.GetAddressOf());
        }

        if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
        {
            // Unordered Access View
            D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

            DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &desc, m_UAV.GetAddressOf());
        }
    }

    return S_OK;
}

int CTexture::Load(const wstring& FilePath)
{
    // 아래 함수 모두 DirectXTex 라이브러리에 있는 기능들
    // 확장자에 알맞게 로드를 해준다.
    
    wchar_t Ext[50] = {};
    _wsplitpath_s(FilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, Ext, 50);

    wstring strExt = Ext;

    HRESULT hr = E_FAIL;

    if (strExt == L".dds" || strExt == L".DDS")
    {
        // .dds .DDS
        hr = LoadFromDDSFile(FilePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_Image);
    }
    else if (strExt == L".tga" || strExt == L".TGA")
    {
        // .tag. .TGA
        hr = LoadFromTGAFile(FilePath.c_str(), nullptr, m_Image);
    }
    else
    {
        // .png .jpg .jpeg .bmp
        hr = LoadFromWICFile(FilePath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, m_Image);
    }

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"텍스쳐 로딩 실패", L"텍스쳐 로딩 실패", MB_OK);

        return E_FAIL;
    }

    // 이미지 객체가 들고있는 정보들을 GPU 메모리에 전달해줘야 함

    // Texture2D Descrition 작성해서 Texture2D 객체 생성


    // Texture2D 객체 이용해서 ShaderResourceView 생성
    // DirectX Engine 에서 그냥 한번에 구현할 수 있게 해주는 함수가 존재함
    hr = CreateShaderResourceView(DEVICE
                            , m_Image.GetImages()
                            , m_Image.GetImageCount()
                            , m_Image.GetMetadata()
                            , m_SRV.GetAddressOf());

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Shader Resource View 생성 실패", L"텍스쳐 로딩 실패", MB_OK);

        return E_FAIL;
    }

    // Texture 2D 로 넘겨주기 ( 한번에 구현을 해버렸기 때문에 꺼내서 다시 넣어줘야함 )
    m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());
    m_Tex2D->GetDesc(&m_Desc);

    return hr;
}

int CTexture::Save(const wstring& FilePath)
{
    return 0;
}
