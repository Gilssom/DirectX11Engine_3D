#include "pch.h"
#include "CDevice.h"

#include "CConstBuffer.h"

#include "CAssetManager.h"

CDevice::CDevice()
	: m_hMainHwnd(nullptr)
	, m_CB{}
	, m_Sampler{}
{
	
}

CDevice::~CDevice()
{
	Safe_Del_Array(m_CB);
}

int CDevice::Init(HWND hwnd, Vec2 resolution)
{
	m_hMainHwnd = hwnd;
	m_RenderResolution = resolution;

	// Global Resolution
	g_GlobalData.vResolution = m_RenderResolution;

	UINT iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE
		, nullptr, iFlag, nullptr, 0, D3D11_SDK_VERSION
		, m_Device.GetAddressOf(), &level, m_Context.GetAddressOf()))) // GetAddressOf() 멤버 변수 이중포인터 반환
	{
		return E_FAIL;
	}

	// Swap Chain
	if (FAILED(CreateSwapChain()))
	{
		return E_FAIL;
	}

	// View
	if (FAILED(CreateView()))
	{
		return E_FAIL;
	}

	// View Port 설정
	// 윈도우 화면에 보여질 영역을 설정한다.
	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (float)m_RenderResolution.x;
	viewPort.Height = (float)m_RenderResolution.y;

	// 깊이 텍스처에 저장되는 깊이 Min ~ Max 지정 ( 0.f ~ 1.f )
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	// ViewPort 정보 세팅
	m_Context->RSSetViewports(1, &viewPort);

	// 필요한 상수버퍼 생성
	if (FAILED(CreateConstBuffer()))
	{
		return E_FAIL;
	}

	// 필요한 샘플러 생성
	if (FAILED(CreateSamplerState()))
	{
		return E_FAIL;
	}

	// Rasterizer State 생성
	if (FAILED(CreateRasterizerState()))
	{
		return E_FAIL;
	}

	// Depth Stencil State 생성
	if (FAILED(CreateDepthStencilState()))
	{
		return E_FAIL;
	}

	// Blend State 생성
	if (FAILED(CreateBlendState()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CDevice::ClearTarget(float(&ArrColor)[4])
{
	m_Context->ClearRenderTargetView(m_RTTex->GetRTV().Get(), ArrColor);

	// Depth Stencil 에서 자신의 깊이 값보다 작은 값들의 물체들만 그려줌
	// 그래서 Depth Stencil View 의 초기값은 1로 초기화 해야 함
	m_Context->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_DEPTH, 1.f, 0);
}

int CDevice::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc = {};

	// Swap Chain 출력 윈도우
	desc.OutputWindow = m_hMainHwnd;
	desc.Windowed = true; // 창모드

	// RenderTarget 텍스쳐의 해상도 설정
	desc.BufferCount = 1; // 생성되는 백버퍼의 갯수 ( API 의 더블버퍼링 렌더와 비슷한 개념 )
	desc.BufferDesc.Width = m_RenderResolution.x;
	desc.BufferDesc.Height = m_RenderResolution.y;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bit
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	desc.BufferDesc.RefreshRate.Denominator = 1; // 부모
	desc.BufferDesc.RefreshRate.Numerator = 60;  // 분모 = 60 ( 재생 비율 )
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 기본 옵션
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 기본 옵션

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = 0;

	ComPtr<IDXGIDevice>  pDXGIDevice = nullptr;
	ComPtr<IDXGIAdapter> pAdapter = nullptr;
	ComPtr<IDXGIFactory> pFactory = nullptr;

	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf());
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf());
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());

	// Swap Chain 생성
	if (FAILED(pFactory->CreateSwapChain(m_Device.Get(), &desc, m_SwapChain.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

int CDevice::CreateView()
{
	// RenderTarget View
	// 그래픽스 파이프라인을 통해서 최종 렌더타겟 텍스쳐에 그려줘야 한다.
	// 중간에 정보를 전달해 주는 매개체 (전달자 역할)
	
	// 1. RenderTarget 용 텍스쳐를 SwapChain 으로부터 가져와야 한다. 이미 있는 정보들이라 간단하다. (포인터 형식)
	ComPtr<ID3D11Texture2D> Tex2D = nullptr;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)Tex2D.GetAddressOf());

	// 2. CTextrue 로 변환
	m_RTTex = CAssetManager::GetInst()->CreateTexture(L"RenderTargetTex", Tex2D);

	// DepthStencil View : 새로운 텍스쳐를 만들어 내야 한다.
	// 1. DepthStencil Texture 생성 ( RenderTarget Tex 와 1:1 비율이여야 한다. )
	m_DSTex = CAssetManager::GetInst()->CreateTexture(L"DepthStencilTex"
													, (UINT)m_RenderResolution.x, (UINT)m_RenderResolution.y
													, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	// Swap Chain 의 텍스처를 렌더타겟으로 지정하고, 
	// 새로 만든 DepthStencil 용 텍스처를 깊이 저장용 텍스처로 지정한다.
	//m_Context->OMSetRenderTargets(1, m_RTTex->GetRTV().GetAddressOf(), m_DSTex->GetDSV().Get()); // 깊이 텍스쳐는 무조건 하나밖에 쓸수 없기 때문에 이중포인터로 안넘긴다.
	
	// 여기서 DepthStencil View 란?
	// Api 에서 더블 버퍼링 시스템으로 화면에 텍스처를 그려주는 방법이 있었다.
	// 누가 먼저 그려질지, 누가 후에 그려질지 각각 설정해줘서 렌더를 진행하는 방법을 사용했다.
	// 하지만 다이렉트에서는 말만 2d 일뿐 좌표계는 3d 이다.
	// 투영 좌표계 (직사각형) 에서 누가 앞에 있고 뒤에 있는지를 확인할 수 있다.
	// DepthStencil View 를 활용해서 깊이 렌더 타겟을 설정하여
	// 해당 텍스처의 stencil 값에 따라 렌더를 진행해주기 때문에 그런 번거로움이 없어지게 된다.

	// ShaderResource View
	// UnorderedAccess View

	return S_OK;
}

#include "CAnim2D.h"

int CDevice::CreateConstBuffer()
{
	m_CB[(UINT)CB_TYPE::TRANSFORM] = new CConstBuffer;
	m_CB[(UINT)CB_TYPE::TRANSFORM]->Create(sizeof(tTransform), CB_TYPE::TRANSFORM);

	m_CB[(UINT)CB_TYPE::MATERIAL] = new CConstBuffer;
	m_CB[(UINT)CB_TYPE::MATERIAL]->Create(sizeof(tMtrlConst), CB_TYPE::MATERIAL);

	m_CB[(UINT)CB_TYPE::ANIMATION] = new CConstBuffer;
	m_CB[(UINT)CB_TYPE::ANIMATION]->Create(sizeof(tAnim2DInfo), CB_TYPE::ANIMATION);

	m_CB[(UINT)CB_TYPE::GLOBAL] = new CConstBuffer;
	m_CB[(UINT)CB_TYPE::GLOBAL]->Create(sizeof(tGlobalData), CB_TYPE::GLOBAL);

	return 0;
}

int CDevice::CreateSamplerState()
{
	D3D11_SAMPLER_DESC desc[2] = {};

	// ANISOTROPIC Sampler - 필터링 작업
	desc[0].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[0].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[0].AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[0].Filter = D3D11_FILTER_ANISOTROPIC;
	DEVICE->CreateSamplerState(desc, m_Sampler[0].GetAddressOf());

	CONTEXT->VSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->HSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->DSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->GSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->PSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->CSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());


	// 저해상도 > 고해상도로 출력할 수 있게 해주는 옵션
	// MIN_MAG_MIP_POINT Sampler
	desc[1].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	DEVICE->CreateSamplerState(desc + 1, m_Sampler[1].GetAddressOf());

	CONTEXT->VSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->HSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->DSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->GSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->PSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->CSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());

	return 0;
}

int CDevice::CreateRasterizerState()
{
	// CULL_BACK 기능은 Default 옵션이기 때문에, nullptr 로 둔다.
	m_RS[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	// CULL_FRONT
	D3D11_RASTERIZER_DESC desc = {};
	desc.CullMode = D3D11_CULL_FRONT;
	desc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&desc, m_RS[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

	// CULL_NONE
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&desc, m_RS[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

	// WIRE_FRAME
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_WIREFRAME;
	DEVICE->CreateRasterizerState(&desc, m_RS[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

	return S_OK;
}

int CDevice::CreateDepthStencilState()
{
	m_DS[(UINT)DS_TYPE::LESS] = nullptr; // 기본 옵션 설정

	D3D11_DEPTH_STENCIL_DESC desc = {};


	// LESS EQUAL
	desc.DepthEnable = true;							// 1. 깊이 비교 기능 사용
	desc.StencilEnable = false;							// 2. Stencil 기능 비활성화
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;		// 3. 작거나 같은 경우 통과
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 4. 깊이 테스트 성공 시, 깊이 기록
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());


	// GREATER
	desc.DepthEnable = true;							// 1. 깊이 비교 기능 사용
	desc.StencilEnable = false;							// 2. Stencil 기능 비활성화
	desc.DepthFunc = D3D11_COMPARISON_GREATER;			// 3. 큰 같은 경우 통과
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 4. 깊이 테스트 성공 시, 깊이 기록
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::GREATER].GetAddressOf());


	// NO_WRITE
	desc.DepthEnable = true;							// 1. 깊이 비교 기능 사용
	desc.StencilEnable = false;							// 2. Stencil 기능 비활성화
	desc.DepthFunc = D3D11_COMPARISON_LESS;				// 3. 작은 경우 통과
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// 4. 깊이 테스트 성공 시, 깊이 기록 X
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());


	// NO_TEST
	desc.DepthEnable = true;							// 1. 깊이 비교 기능 사용
	desc.StencilEnable = false;							// 2. Stencil 기능 비활성화
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;			// 3. 항상 통과
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 4. 깊이 테스트 성공 시, 깊이 기록
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::NO_TEST].GetAddressOf());


	// NO_TEST_NO_WRITE
	desc.DepthEnable = false;							// 1. 깊이 비교 기능 미사용
	desc.StencilEnable = false;							// 2. Stencil 기능 비활성화
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;			// 3. 항상 통과
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// 4. 깊이 기록 X
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());


	return S_OK;
}

int CDevice::CreateBlendState()
{
	m_BS[(UINT)BS_TYPE::DEFAULT] = nullptr;

	D3D11_BLEND_DESC desc = {};


	// ALPHA_BLEND_NOCOVERAGE
	desc.AlphaToCoverageEnable = false; // 풀, 나뭇잎 같은 경우에 사용할 때
	// Direct 내부에서 Alpha 값의 깊이를 모두 보정을 해줌

	// 독립적인 기능 ( 렌더 타겟이 여러개 일 상황이 있을 수 있어서 ) ( 최대 8장 )
	// 꺼놓으면 모든 Render Target 은 0번 옵션으로 출력이 된다.
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true; // 블렌딩 기능 사용

	// 블렌딩 방식
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	// 블렌딩 계수
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;						// 본인의 Alpha 값을 사용하겠다.
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;					// Source 의 Alpha 의 역수를 사용하겠다.
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// 최종 출력

	// 알파 끼리의 혼합식
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	HRESULT hr = DEVICE->CreateBlendState(&desc, m_BS[(UINT)BS_TYPE::ALPHA_BLEND].GetAddressOf());


	// ALPHA_BLEND_COVERAGE
	desc.AlphaToCoverageEnable = true; // 풀, 나뭇잎 같은 경우에 사용할 때
	// Direct 내부에서 Alpha 값의 깊이를 모두 보정을 해줌

	// 독립적인 기능 ( 렌더 타겟이 여러개 일 상황이 있을 수 있어서 ) ( 최대 8장 )
	// 꺼놓으면 모든 Render Target 은 0번 옵션으로 출력이 된다.
	desc.IndependentBlendEnable = false; 

	desc.RenderTarget[0].BlendEnable = true; // 블렌딩 기능 사용

	// 블렌딩 방식
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	// 블렌딩 계수
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;						// 본인의 Alpha 값을 사용하겠다.
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;					// Source 의 Alpha 의 역수를 사용하겠다.
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// 최종 출력

	// 알파 끼리의 혼합식
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	hr = DEVICE->CreateBlendState(&desc, m_BS[(UINT)BS_TYPE::ALPHA_BLEND_COVERAGE].GetAddressOf());


	// ONE_ONE
	desc.AlphaToCoverageEnable = false; 
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;							// 비율 = 1.
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;							// 비율 = 1.
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// 최종 출력

	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	DEVICE->CreateBlendState(&desc, m_BS[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());

	return S_OK;
}
