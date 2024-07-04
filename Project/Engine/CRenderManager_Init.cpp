#include "pch.h"
#include "CRenderManager.h"

#include "CDevice.h"

#include "CMRT.h"

void CRenderManager::Init()
{
	// RenderTarget �� ������� �� �ִ� �ؽ��ĸ� ����
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();
	m_RenderTargetCopyTex = CAssetManager::GetInst()->CreateTexture(L"RenderTargetCopyTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);

	// MRT ����
	CreateMRT();
}

void CRenderManager::CreateMRT()
{
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	CMRT* pMRT = nullptr;

	// ================
	//	SwapChain MRT
	// ================
	pMRT = m_MRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;

	Ptr<CTexture> pRTTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");

	Ptr<CTexture> DSTex = CAssetManager::GetInst()->CreateTexture(L"DepthStencilTex"
											, (UINT)vResolution.x, (UINT)vResolution.y
											, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	pMRT->Create(&pRTTex, 1, DSTex);


	// ================
	//	Deferred MRT
	// ================
	{
		pMRT = m_MRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;

		Ptr<CTexture> arrRTTex[8] =
		{
			// 1. Color Target (��ü�� ���� ����, ���� X)
			CAssetManager::GetInst()->CreateTexture(L"ColorTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R8G8B8A8_UNORM
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 2. Normal Target (��ü ǥ�� Normal ���� ����)
			CAssetManager::GetInst()->CreateTexture(L"NormalTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 3. Position Target (��ü�� ��ġ ���� ����)
			CAssetManager::GetInst()->CreateTexture(L"PositionTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 4. Emissive Target (��ü �߱� ����)
			CAssetManager::GetInst()->CreateTexture(L"EmissiveTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 5. Data Target (�� �� ����� ���� ����)
			CAssetManager::GetInst()->CreateTexture(L"DataTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		pMRT->Create(arrRTTex, 5, DSTex);
	}
}

void CRenderManager::ClearMRT()
{
	// Swap Chain �� ��� Clear
	m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->ClearTarget();
	m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->ClearDepthStencil();

	// Deferred �� Render Target �� Clear
	m_MRT[(UINT)MRT_TYPE::DEFERRED]->ClearTarget();
}