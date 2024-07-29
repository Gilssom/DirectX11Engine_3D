#pragma once
#include "CAsset.h"

class CTexture : public CAsset
{
private:
	ScratchImage						m_Image; // Load �Լ��� ������ �� ���� ( �ý��� �޸� )
	ComPtr<ID3D11Texture2D>				m_Tex2D; // GPU �޸�

	ComPtr<ID3D11RenderTargetView>		m_RTV;
	ComPtr<ID3D11DepthStencilView>		m_DSV;
	ComPtr<ID3D11ShaderResourceView>	m_SRV;	 // GPU �޸𸮿� ���޽�Ű�� ���� �ʿ��� View

	// unordered Texture �� ���޽�Ű�� ���� �ʿ��� View
	// ����ҷ��� Texture �� �����Ǵ� ó������ ������ ���־����.
	ComPtr<ID3D11UnorderedAccessView>	m_UAV;	 

	D3D11_TEXTURE2D_DESC				m_Desc;  // Animation �� �����ϱ� ���� Texture ������ ���޽�Ű�� ����

public:
	const D3D11_TEXTURE2D_DESC& GetDesc() { return m_Desc; }
	float GetWidth() { return (float)m_Desc.Width; }
	float GetHeight() { return (float)m_Desc.Height; }
	void* GetSysMem() { return m_Image.GetPixels(); }
	const TexMetadata& GetMetaData() { return m_Image.GetMetadata(); }
	size_t GetRowPitch() { return m_Image.GetImages()->rowPitch; }
	size_t GetSlicePitch() { return m_Image.GetImages()->slicePitch; }

	ComPtr<ID3D11Texture2D>				GetTex2D()	{ return m_Tex2D; }
	ComPtr<ID3D11RenderTargetView>		GetRTV()	{ return m_RTV; }
	ComPtr<ID3D11DepthStencilView>		GetDSV()	{ return m_DSV; }
	ComPtr<ID3D11ShaderResourceView>	GetSRV()	{ return m_SRV; }
	ComPtr<ID3D11UnorderedAccessView>	GetUAV()	{ return m_UAV; }


public:
	// Binding �� Clear �� �б� ó��
	void Binding(int registerNum);
	void Binding_CS_SRV(int registerNum);
	void Binding_CS_UAV(int registerNum);

	static void Clear(int registerNum);
	static void Clear_CS_SRV(int registerNum);
	static void Clear_CS_UAV(int registerNum);


private:
	virtual int Load(const wstring& FilePath) override;
	virtual int Save(const wstring& FilePath) override;


public:
	// bindFlag : D3D11_BIND_FLAG
	int Create(UINT width, UINT height, DXGI_FORMAT pixelFormat, UINT bindFlag, D3D11_USAGE usage);
	int Create(ComPtr<ID3D11Texture2D> tex2D);
	int CreateArrayTexture(const vector<Ptr<CTexture>>& vecTex);


public:
	CLONE_DISABLE(CTexture);


public:
	CTexture(bool bEngine = false);
	~CTexture();

	friend class CAssetManager;
};

