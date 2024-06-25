#pragma once

#include "CTexture.h"

class CConstBuffer;

class CDevice : public CSingleton<CDevice>
{
	SINGLE(CDevice)

private:
	HWND					m_hMainHwnd;		// 출력 대상 윈도우
	Vec2					m_RenderResolution; // 렌더링 타겟 해상도
	
	// ComPtr : 스마트포인터 ( 레퍼런스 관리 )
	// 직접 초기화를 할 필요가 없다. 클래스라서 생성자에서 직접 해주기 때문
	// 따라서 소멸자도 구현이 되어 있기 때문에 Release 를 직접 해줄 필요가 없다.
	// 포인터 주소를 넘겨주는 Get 함수와
	// 이중포인터 주소를 넘겨주는 GetAddressOf 함수가 구현되어 있다.
	ComPtr<ID3D11Device>			m_Device;		// GPU 메모리 할당, Dx11 관련 객체 생성			
	ComPtr<ID3D11DeviceContext>		m_Context;		// GPU 렌더링 관련 명령,
	ComPtr<IDXGISwapChain>			m_SwapChain;	// 렌더타겟 버퍼 소유, 화면에 최종 장면을 게시

	Ptr<CTexture>					m_RTTex;
	Ptr<CTexture>					m_DSTex;

	ComPtr<ID3D11SamplerState>		m_Sampler[2];
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
	void ClearTarget(float(&ArrColor)[4]);
	void Present() { m_SwapChain->Present(0, 0); }

	Vec2 GetRenderResolution() { return m_RenderResolution; }

	ID3D11Device* GetDevice() { return m_Device.Get(); }
	ID3D11DeviceContext* GetContext() { return m_Context.Get(); }

	CConstBuffer* GetConstBuffer(CB_TYPE type) { return m_CB[(UINT)type]; }
	ComPtr<ID3D11RasterizerState> GetRS(RS_TYPE type) { return m_RS[(UINT)type]; }
	ComPtr<ID3D11DepthStencilState> GetDS(DS_TYPE type) { return m_DS[(UINT)type]; }
	ComPtr<ID3D11BlendState> GetBS(BS_TYPE type) { return m_BS[(UINT)type]; }
};