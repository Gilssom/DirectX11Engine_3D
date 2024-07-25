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
		, m_Device.GetAddressOf(), &level, m_Context.GetAddressOf()))) // GetAddressOf() ��� ���� ���������� ��ȯ
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

	// View Port ����
	// MRT �̵�

	// �ʿ��� ������� ����
	if (FAILED(CreateConstBuffer()))
	{
		return E_FAIL;
	}

	// �ʿ��� ���÷� ����
	if (FAILED(CreateSamplerState()))
	{
		return E_FAIL;
	}

	// Rasterizer State ����
	if (FAILED(CreateRasterizerState()))
	{
		return E_FAIL;
	}

	// Depth Stencil State ����
	if (FAILED(CreateDepthStencilState()))
	{
		return E_FAIL;
	}

	// Blend State ����
	if (FAILED(CreateBlendState()))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CDevice::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc = {};

	// Swap Chain ��� ������
	desc.OutputWindow = m_hMainHwnd;
	desc.Windowed = true; // â���

	// RenderTarget �ؽ����� �ػ� ����
	desc.BufferCount = 1; // �����Ǵ� ������� ���� ( API �� ������۸� ������ ����� ���� )
	desc.BufferDesc.Width = m_RenderResolution.x;
	desc.BufferDesc.Height = m_RenderResolution.y;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bit
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	desc.BufferDesc.RefreshRate.Denominator = 1; // �θ�
	desc.BufferDesc.RefreshRate.Numerator = 60;  // �и� = 60 ( ��� ���� )
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // �⺻ �ɼ�
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // �⺻ �ɼ�

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

	// Swap Chain ����
	if (FAILED(pFactory->CreateSwapChain(m_Device.Get(), &desc, m_SwapChain.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

int CDevice::CreateView()
{
	// RenderTarget View
	// �׷��Ƚ� ������������ ���ؼ� ���� ����Ÿ�� �ؽ��Ŀ� �׷���� �Ѵ�.
	// �߰��� ������ ������ �ִ� �Ű�ü (������ ����)
	
	// 1. RenderTarget �� �ؽ��ĸ� SwapChain ���κ��� �����;� �Ѵ�. �̹� �ִ� �������̶� �����ϴ�. (������ ����)
	ComPtr<ID3D11Texture2D> Tex2D = nullptr;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)Tex2D.GetAddressOf());

	// 2. CTextrue �� ��ȯ (Render Target Texture Asset ���)
	CAssetManager::GetInst()->CreateTexture(L"RenderTargetTex", Tex2D);

	// DepthStencil View : ���ο� �ؽ��ĸ� ����� ���� �Ѵ�.
	// 1. DepthStencil Texture ���� ( RenderTarget Tex �� 1:1 �����̿��� �Ѵ�. )
	// Swap Chain �� �ؽ�ó�� ����Ÿ������ �����ϰ�, 
	// ���� ���� DepthStencil �� �ؽ�ó�� ���� ����� �ؽ�ó�� �����Ѵ�.

	// ��>>> MRT ���� �ڵ�� �̵�

	// ���⼭ DepthStencil View ��?
	// Api ���� ���� ���۸� �ý������� ȭ�鿡 �ؽ�ó�� �׷��ִ� ����� �־���.
	// ���� ���� �׷�����, ���� �Ŀ� �׷����� ���� �������༭ ������ �����ϴ� ����� ����ߴ�.
	// ������ ���̷�Ʈ������ ���� 2d �ϻ� ��ǥ��� 3d �̴�.
	// ���� ��ǥ�� (���簢��) ���� ���� �տ� �ְ� �ڿ� �ִ����� Ȯ���� �� �ִ�.
	// DepthStencil View �� Ȱ���ؼ� ���� ���� Ÿ���� �����Ͽ�
	// �ش� �ؽ�ó�� stencil ���� ���� ������ �������ֱ� ������ �׷� ���ŷο��� �������� �ȴ�.

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

	// ANISOTROPIC Sampler - ���͸� �۾�
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


	// ���ػ� > ���ػ󵵷� ����� �� �ְ� ���ִ� �ɼ�
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
	// CULL_BACK ����� Default �ɼ��̱� ������, nullptr �� �д�.
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
	m_DS[(UINT)DS_TYPE::LESS] = nullptr; // �⺻ �ɼ� ����

	D3D11_DEPTH_STENCIL_DESC desc = {};


	// LESS EQUAL
	desc.DepthEnable = true;							// 1. ���� �� ��� ���
	desc.StencilEnable = false;							// 2. Stencil ��� ��Ȱ��ȭ
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;		// 3. �۰ų� ���� ��� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 4. ���� �׽�Ʈ ���� ��, ���� ���
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());


	// GREATER
	desc.DepthEnable = true;							// 1. ���� �� ��� ���
	desc.StencilEnable = false;							// 2. Stencil ��� ��Ȱ��ȭ
	desc.DepthFunc = D3D11_COMPARISON_GREATER;			// 3. ū ���� ��� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 4. ���� �׽�Ʈ ���� ��, ���� ���
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::GREATER].GetAddressOf());


	// NO_WRITE
	desc.DepthEnable = true;							// 1. ���� �� ��� ���
	desc.StencilEnable = false;							// 2. Stencil ��� ��Ȱ��ȭ
	desc.DepthFunc = D3D11_COMPARISON_LESS;				// 3. ���� ��� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// 4. ���� �׽�Ʈ ���� ��, ���� ��� X
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());


	// NO_TEST
	desc.DepthEnable = true;							// 1. ���� �� ��� ���
	desc.StencilEnable = false;							// 2. Stencil ��� ��Ȱ��ȭ
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;			// 3. �׻� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 4. ���� �׽�Ʈ ���� ��, ���� ���
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::NO_TEST].GetAddressOf());


	// NO_TEST_NO_WRITE
	desc.DepthEnable = false;							// 1. ���� �� ��� �̻��
	desc.StencilEnable = false;							// 2. Stencil ��� ��Ȱ��ȭ
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;			// 3. �׻� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// 4. ���� ��� X
	DEVICE->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());


	// BackFace Check
	desc.DepthEnable = true;
	desc.DepthFunc = D3D11_COMPARISON_GREATER;				// ���̰� �� �ָ� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	desc.StencilEnable = true;								// ���ٽ� ��� ��Ȱ��ȭ
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// ���ٽ� �˻�� ���� ����
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;


	// FrontFace Check
	desc.DepthEnable = true;
	desc.DepthFunc = D3D11_COMPARISON_LESS;					// ���̰� �� ������� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	desc.StencilEnable = true;								// ���ٽ� ��� ��Ȱ��ȭ
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// ���ٽ� �˻�� ���� ����
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_DECR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;


	// BackFace, FrontFace, RS_TYPE::CULL_NONE
	desc.DepthEnable = true;
	desc.DepthFunc = D3D11_COMPARISON_GREATER;				// ���̰� �� �ָ� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	
	desc.StencilEnable = true;								// ���ٽ� ��� ��Ȱ��ȭ
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// ���ٽ� �˻�� ���� ����
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// ���ٽ� �˻�� ���� ����
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;



	// Stencil Check
	desc.DepthEnable = false;								// ���� Test ��� ����
	desc.DepthFunc = D3D11_COMPARISON_NEVER;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	
	desc.StencilEnable = true;								// ���ٽ� ��� ��Ȱ��ȭ
	desc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER;	// Ư�� ���ٽ� ���� ���� ��츸 ���
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;

	return S_OK;
}

int CDevice::CreateBlendState()
{
	m_BS[(UINT)BS_TYPE::DEFAULT] = nullptr;

	D3D11_BLEND_DESC desc = {};


	// ALPHA_BLEND_NOCOVERAGE
	desc.AlphaToCoverageEnable = false; // Ǯ, ������ ���� ��쿡 ����� ��
	// Direct ���ο��� Alpha ���� ���̸� ��� ������ ����

	// �������� ��� ( ���� Ÿ���� ������ �� ��Ȳ�� ���� �� �־ ) ( �ִ� 8�� )
	// �������� ��� Render Target �� 0�� �ɼ����� ����� �ȴ�.
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true; // ���� ��� ���

	// ���� ���
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	// ���� ���
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;						// ������ Alpha ���� ����ϰڴ�.
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;					// Source �� Alpha �� ������ ����ϰڴ�.
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// ���� ���

	// ���� ������ ȥ�ս�
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	HRESULT hr = DEVICE->CreateBlendState(&desc, m_BS[(UINT)BS_TYPE::ALPHA_BLEND].GetAddressOf());


	// ==========================
	// ALPHA_BLEND_COVERAGE
	// ==========================
	desc.AlphaToCoverageEnable = true; // Ǯ, ������ ���� ��쿡 ����� ��
	// Direct ���ο��� Alpha ���� ���̸� ��� ������ ����

	// �������� ��� ( ���� Ÿ���� ������ �� ��Ȳ�� ���� �� �־ ) ( �ִ� 8�� )
	// �������� ��� Render Target �� 0�� �ɼ����� ����� �ȴ�.
	desc.IndependentBlendEnable = false; 

	desc.RenderTarget[0].BlendEnable = true; // ���� ��� ���

	// ���� ���
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	// ���� ���
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;						// ������ Alpha ���� ����ϰڴ�.
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;					// Source �� Alpha �� ������ ����ϰڴ�.
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// ���� ���

	// ���� ������ ȥ�ս�
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	hr = DEVICE->CreateBlendState(&desc, m_BS[(UINT)BS_TYPE::ALPHA_BLEND_COVERAGE].GetAddressOf());


	// ==========================
	// ONE_ONE
	// ==========================
	desc.AlphaToCoverageEnable = false; 
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;							// ���� = 1.
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;							// ���� = 1.
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// ���� ���

	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	DEVICE->CreateBlendState(&desc, m_BS[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());

	return S_OK;


	// ==============================================
	// Decal Blend :: 0 -> Alpha Blend, 1 -> One One
	// ==============================================
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = true;

	// ���� ���
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	// ���� ���
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// ���ĳ����� ȥ�ս�
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	// ���� ���
	desc.RenderTarget[1].BlendEnable = true;
	desc.RenderTarget[1].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	// ���� ���
	desc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[1].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// ���ĳ����� ȥ�ս�
	desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;

	DEVICE->CreateBlendState(&desc, m_BS[(UINT)BS_TYPE::DECAL_BLEND].GetAddressOf());

	return S_OK;
}
