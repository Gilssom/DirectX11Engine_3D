#pragma once

#include "CTexture.h"

class CConstBuffer;

class CDevice : public CSingleton<CDevice>
{
	SINGLE(CDevice)

private:
	HWND					m_hMainHwnd;		// ��� ��� ������
	Vec2					m_RenderResolution; // ������ Ÿ�� �ػ�
	
	// ComPtr : ����Ʈ������ ( ���۷��� ���� )
	// ���� �ʱ�ȭ�� �� �ʿ䰡 ����. Ŭ������ �����ڿ��� ���� ���ֱ� ����
	// ���� �Ҹ��ڵ� ������ �Ǿ� �ֱ� ������ Release �� ���� ���� �ʿ䰡 ����.
	// ������ �ּҸ� �Ѱ��ִ� Get �Լ���
	// ���������� �ּҸ� �Ѱ��ִ� GetAddressOf �Լ��� �����Ǿ� �ִ�.
	ComPtr<ID3D11Device>			m_Device;		// GPU �޸� �Ҵ�, Dx11 ���� ��ü ����			
	ComPtr<ID3D11DeviceContext>		m_Context;		// GPU ������ ���� ���,
	ComPtr<IDXGISwapChain>			m_SwapChain;	// ����Ÿ�� ���� ����, ȭ�鿡 ���� ����� �Խ�

	ComPtr<ID3D11SamplerState>		m_Sampler[3];
	ComPtr<ID3D11RasterizerState>	m_RS[(UINT)RS_TYPE::END];

	ComPtr<ID3D11BlendState>		m_BS[(UINT)BS_TYPE::END];
	ComPtr<ID3D11DepthStencilState> m_DS[(UINT)DS_TYPE::END];
	CConstBuffer*					m_CB[(UINT)CB_TYPE::END];

private:
	int CreateSwapChain();
	int CreateView();
	int CreateConstBuffer();
	int CreateSamplerState();
	int CreateRasterizerState();
	int CreateDepthStencilState();
	int CreateBlendState();

public:
	int Init(HWND hwnd, Vec2 resolution);
	void Present() { m_SwapChain->Present(0, 0); }

	Vec2 GetRenderResolution() { return m_RenderResolution; }

	ID3D11Device* GetDevice() { return m_Device.Get(); }
	ID3D11DeviceContext* GetContext() { return m_Context.Get(); }

	CConstBuffer* GetConstBuffer(CB_TYPE type) { return m_CB[(UINT)type]; }
	ComPtr<ID3D11RasterizerState> GetRS(RS_TYPE type) { return m_RS[(UINT)type]; }
	ComPtr<ID3D11DepthStencilState> GetDS(DS_TYPE type) { return m_DS[(UINT)type]; }
	ComPtr<ID3D11BlendState> GetBS(BS_TYPE type) { return m_BS[(UINT)type]; }
};