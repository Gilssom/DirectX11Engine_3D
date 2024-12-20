#pragma once
#include "CAsset.h"

class CTexture : public CAsset
{
private:
	ScratchImage						m_Image; // Load 함수를 구현할 수 있음 ( 시스템 메모리 )
	ComPtr<ID3D11Texture2D>				m_Tex2D; // GPU 메모리

	ComPtr<ID3D11RenderTargetView>		m_RTV;
	ComPtr<ID3D11DepthStencilView>		m_DSV;
	ComPtr<ID3D11ShaderResourceView>	m_SRV;	 // GPU 메모리에 전달시키기 위해 필요한 View

	// unordered Texture 에 전달시키기 위해 필요한 View
	// 사용할려면 Texture 가 생성되는 처음부터 지정을 해주어야함.
	ComPtr<ID3D11UnorderedAccessView>	m_UAV;	 

	D3D11_TEXTURE2D_DESC				m_Desc;  // Animation 을 제작하기 위해 Texture 정보를 전달시키기 위함

public:
	const D3D11_TEXTURE2D_DESC& GetDesc() { return m_Desc; }
	float GetWidth() { return (float)m_Desc.Width; }
	float GetHeight() { return (float)m_Desc.Height; }
	void* GetSysMem() { return m_Image.GetPixels(); }
	const TexMetadata& GetMetaData() { return m_Image.GetMetadata(); }
	size_t GetRowPitch() { return m_Image.GetImages()->rowPitch; }
	size_t GetSlicePitch() { return m_Image.GetImages()->slicePitch; }
	UINT GetArraySize() { return m_Desc.ArraySize; }

	ComPtr<ID3D11Texture2D>				GetTex2D()	{ return m_Tex2D; }
	ComPtr<ID3D11RenderTargetView>		GetRTV()	{ return m_RTV; }
	ComPtr<ID3D11DepthStencilView>		GetDSV()	{ return m_DSV; }
	ComPtr<ID3D11ShaderResourceView>	GetSRV()	{ return m_SRV; }
	ComPtr<ID3D11UnorderedAccessView>	GetUAV()	{ return m_UAV; }


public:
	// Binding 과 Clear 의 분기 처리
	void Binding(int registerNum);
	void Binding_CS_SRV(int registerNum);
	void Binding_CS_UAV(int registerNum);

	static void Clear(int registerNum);
	static void Clear_CS_SRV(int registerNum);
	static void Clear_CS_UAV(int registerNum);


private:
	virtual int Load(const wstring& FilePath) override;

public:
	virtual int Save(const wstring& FilePath) override;


public:
	// bindFlag : D3D11_BIND_FLAG
	int Create(UINT width, UINT height, DXGI_FORMAT pixelFormat, UINT bindFlag, D3D11_USAGE usage);
	int Create(ComPtr<ID3D11Texture2D> tex2D);
	int CreateArrayTexture(const vector<Ptr<CTexture>>& vecTex);

	int GenerateMip(UINT mipLevel);

public:
	CLONE_DISABLE(CTexture);


public:
	CTexture(bool bEngine = false);
	~CTexture();

	friend class CAssetManager;
};

