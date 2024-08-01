#include "pch.h"
#include "CTexture.h"

#include "CDevice.h"
#include "CPathManager.h"

CTexture::CTexture(bool bEngine)
    : CAsset(ASSET_TYPE::TEXTURE, bEngine)
    , m_Desc{}
{
}

CTexture::~CTexture()
{
}

void CTexture::Binding(int registerNum)
{
    CONTEXT->VSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
    CONTEXT->HSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
    CONTEXT->DSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
    CONTEXT->GSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
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
    CONTEXT->VSSetShaderResources(registerNum, 1, &pSRV);
    CONTEXT->HSSetShaderResources(registerNum, 1, &pSRV);
    CONTEXT->DSSetShaderResources(registerNum, 1, &pSRV);
    CONTEXT->GSSetShaderResources(registerNum, 1, &pSRV);
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

    // Bind Flag �� ������ �� ���� �ִ�. (���� �� Ȯ�� �ʿ�) ������ ��Ʈ�� ����Ǿ� ���� (����Ÿ������ �ް� ��)
    // D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER ...
    m_Desc.BindFlags = bindFlag;
    m_Desc.MipLevels = 1;
    m_Desc.SampleDesc.Count = 1;
    m_Desc.SampleDesc.Quality = 0;

    if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, &m_Tex2D)))
    {
        return E_FAIL;
    }

    // ��Ʈ �����ڷ� �ش� Bing Flag Type Ȯ��
    if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
    }
    // Depth Stencil Flag �� ���� Flag �̿��� �ϱ� ������ else ó�� (�ٸ� �Ͱ� ������ �Ұ���)
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
    // �̹� ������� Texture ���� ��������
    m_Tex2D = tex2D;
    m_Tex2D->GetDesc(&m_Desc);

    // ��Ʈ �����ڷ� �ش� Bing Flag Type Ȯ��
    if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
    }
    // Depth Stencil Flag �� ���� Flag �̿��� �ϱ� ������ else ó�� (�ٸ� �Ͱ� ������ �Ұ���)
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

int CTexture::CreateArrayTexture(const vector<Ptr<CTexture>>& vecTex)
{
    m_Desc = vecTex[0]->GetDesc();
    m_Desc.ArraySize = (UINT)vecTex.size();
    m_Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
    m_Desc.MipLevels = 1;

    if(FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
    {
        return E_FAIL;
    }

    // �� ���� Texture �� ������ Array Texture �� �� ĭ���� �����Ų��.
    for (size_t i = 0; i < vecTex.size(); i++)
    {
        UINT offset = D3D11CalcSubresource(0, i, 1);
        CONTEXT->UpdateSubresource(m_Tex2D.Get(), offset, nullptr
                                 , vecTex[i]->GetSysMem()
                                 , vecTex[i]->GetRowPitch()
                                 , vecTex[i]->GetSlicePitch());
    }

    // Shader Resource View ����
    D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

    tSRVDesc.Format                         = m_Desc.Format;
    tSRVDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    tSRVDesc.Texture2DArray.MipLevels       = 1;
    tSRVDesc.Texture2DArray.MostDetailedMip = 0;
    tSRVDesc.Texture2DArray.ArraySize       = m_Desc.ArraySize;

    if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf())))
    {
        E_FAIL;
    }
    
    return 0;
}

int CTexture::Load(const wstring& FilePath)
{
    // �Ʒ� �Լ� ��� DirectXTex ���̺귯���� �ִ� ��ɵ�
    // Ȯ���ڿ� �˸°� �ε带 ���ش�.
    
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
        MessageBox(nullptr, L"�ؽ��� �ε� ����", L"�ؽ��� �ε� ����", MB_OK);

        return E_FAIL;
    }

    // �̹��� ��ü�� ����ִ� �������� GPU �޸𸮿� ��������� ��

    // Texture2D Descrition �ۼ��ؼ� Texture2D ��ü ����


    // Texture2D ��ü �̿��ؼ� ShaderResourceView ����
    // DirectX Engine ���� �׳� �ѹ��� ������ �� �ְ� ���ִ� �Լ��� ������
    hr = CreateShaderResourceView(DEVICE
                            , m_Image.GetImages()
                            , m_Image.GetImageCount()
                            , m_Image.GetMetadata()
                            , m_SRV.GetAddressOf());

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Shader Resource View ���� ����", L"�ؽ��� �ε� ����", MB_OK);

        return E_FAIL;
    }

    // Texture 2D �� �Ѱ��ֱ� ( �ѹ��� ������ �ع��ȱ� ������ ������ �ٽ� �־������ )
    m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());
    m_Tex2D->GetDesc(&m_Desc);

    return hr;
}

int CTexture::Save(const wstring& FilePath)
{
    // GPU -> System
    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);

    // System -> File
    wstring strRelativePath = CPathManager::GetInst()->GetRelativePath(FilePath);
    SetRelativePath(strRelativePath);

    HRESULT hr = E_FAIL;

    if (m_Image.GetMetadata().arraySize == 1)
    {
        // png, jpg, jpeg, bmp
        hr = SaveToWICFile(*m_Image.GetImages()
                    , WIC_FLAGS_NONE
                    , GetWICCodec(WICCodecs::WIC_CODEC_PNG)
                    , FilePath.c_str());    
    }
    else
    {
        hr = SaveToDDSFile(m_Image.GetImages()
                    , m_Image.GetMetadata().arraySize
                    , m_Image.GetMetadata()
                    , DDS_FLAGS_NONE
                    , FilePath.c_str());
    }

    return hr;
}