#pragma once
#include "CAsset.h"

struct tShaderScalarParam
{
    string          strDesc;
    SCALAR_PARAM    Param;
};

struct tShaderTexParam
{
    string      strDesc;
    TEX_PARAM   Param;
};

class CShader : public CAsset
{
protected:
    // Error Blob
    ComPtr<ID3DBlob>		    m_ErrBlob;
    vector<tShaderScalarParam>  m_vecScalar;
    vector<tShaderTexParam>     m_vecTex;

public:
    void AddScalarParam(const string& strDesc, SCALAR_PARAM param) { m_vecScalar.push_back(tShaderScalarParam{ strDesc, param }); }
    void AddTexParam(const string& strDesc, TEX_PARAM param) { m_vecTex.push_back(tShaderTexParam{ strDesc, param }); }

    const vector<tShaderScalarParam>& GetScalarParam() { return m_vecScalar; }
    const vector<tShaderTexParam>& GetTexParam() { return m_vecTex; }

public:
    virtual int Binding() = 0;
    virtual int Load(const wstring& _FilePath) override { return S_OK; }
    virtual int Save(const wstring& _FilePath) override { return S_OK; }


    CLONE_DISABLE(CShader);

public:
    CShader(ASSET_TYPE type);
    ~CShader();
};

