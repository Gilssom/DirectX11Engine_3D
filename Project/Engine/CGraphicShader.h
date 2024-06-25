#pragma once
#include "CShader.h"

class CGraphicShader : public CShader
{
private:
    ComPtr<ID3DBlob>            m_VSBlob;
    ComPtr<ID3DBlob>            m_HSBlob;
    ComPtr<ID3DBlob>            m_DSBlob;
    ComPtr<ID3DBlob>            m_GSBlob;
    ComPtr<ID3DBlob>            m_PSBlob;

    ComPtr<ID3D11VertexShader>      m_VS;
    ComPtr<ID3D11HullShader>        m_HS;
    ComPtr<ID3D11DomainShader>      m_DS;
    ComPtr<ID3D11GeometryShader>    m_GS;
    ComPtr<ID3D11PixelShader>       m_PS;


    // 정점 하나를 구성하는 Layout 정보가 필요함
    ComPtr<ID3D11InputLayout>   m_Layout;

    D3D11_PRIMITIVE_TOPOLOGY    m_Topology;

    RS_TYPE                     m_RSType;
    DS_TYPE                     m_DSType;
    BS_TYPE                     m_BSType;

    SHADER_DOMAIN               m_Domain;

public:
    int CreateVertexShader(const wstring& strFilePath, const string& vsFuncName);
    int CreateHullShader(const wstring& strFilePath, const string& hsFuncName);
    int CreateDomainShader(const wstring& strFilePath, const string& dsFuncName);
    int CreateGeometryShader(const wstring& strFilePath, const string& gsFuncName);
    int CreatePixelShader(const wstring& strFilePath, const string& psFuncName);

    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { m_Topology = topology; }
    void SetRSType(RS_TYPE type) { m_RSType = type; }
    void SetDSType(DS_TYPE type) { m_DSType = type; }
    void SetBSType(BS_TYPE type) { m_BSType = type; }

    virtual int Binding() override;

    void SetDomain(SHADER_DOMAIN domain) { m_Domain = domain; }
    SHADER_DOMAIN GetDomain() { return m_Domain; }

public:
    CGraphicShader();
    ~CGraphicShader();
};

