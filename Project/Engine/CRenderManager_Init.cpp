#include "pch.h"
#include "CRenderManager.h"

#include "CDevice.h"

#include "CMRT.h"

void CRenderManager::Init()
{
	// RenderTarget 을 복사받을 수 있는 텍스쳐를 생성
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();
	m_RenderTargetCopyTex = CAssetManager::GetInst()->CreateTexture(L"RenderTargetCopyTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);

	m_ReflectionTex = CAssetManager::GetInst()->CreateTexture(L"ReflectionTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R32G32B32A32_FLOAT
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	Ptr<CTexture> pTest = CAssetManager::GetInst()->CreateTexture(L"ShadowMapTargetTex"
		, 8192, 8192
		, DXGI_FORMAT_R32_FLOAT
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	// MRT 생성
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
											, DXGI_FORMAT_D24_UNORM_S8_UINT
											, D3D11_BIND_DEPTH_STENCIL);

	pMRT->Create(&pRTTex, 1, DSTex);


	// ================
	//	Deferred MRT
	// ================
	{
		pMRT = m_MRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;

		Ptr<CTexture> arrRTTex[8] =
		{
			// 0. Color Target (물체의 색상만 저장, 광원 X)
			CAssetManager::GetInst()->CreateTexture(L"ColorTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R8G8B8A8_UNORM
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 1. Normal Target (물체 표면 Normal 정보 저장)
			CAssetManager::GetInst()->CreateTexture(L"NormalTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 2. Position Target (물체의 위치 정보 저장)
			CAssetManager::GetInst()->CreateTexture(L"PositionTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 3. Emissive Target (자체 발광 정보)
			CAssetManager::GetInst()->CreateTexture(L"EmissiveTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 4. Data Target (그 외 사용자 정의 정보)
			CAssetManager::GetInst()->CreateTexture(L"DataTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		pMRT->Create(arrRTTex, 5, DSTex);
	}


	// ================
	//	Decal MRT
	// ================
	{
		pMRT = m_MRT[(UINT)MRT_TYPE::DECAL] = new CMRT;

		Ptr<CTexture> arrRTTex[8] =
		{
			// 0. Color Target (물체의 색상만 저장, 광원 X)
			CAssetManager::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"),

			// 1. Emissive Target (자체 발광 정보 저장)
			CAssetManager::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"),
		};

		pMRT->Create(arrRTTex, 2, nullptr);
	}



	// ================
	//	Light MRT
	// ================
	{
		pMRT = m_MRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;

		Ptr<CTexture> arrRTTex[8] =
		{
			// 1. Diffuse Target (물체에 빛이 부딪힌 세기 (난반사 세기))
			CAssetManager::GetInst()->CreateTexture(L"DiffuseTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// 2. Specular Target (물체에서 반사된 빛이 카메라로 들어오는 세기)
			CAssetManager::GetInst()->CreateTexture(L"SpecularTargetTex"
									, (UINT)vResolution.x, (UINT)vResolution.y
									, DXGI_FORMAT_R32G32B32A32_FLOAT
									, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		// 광원 MRT 는 깊이 텍스처 X
		pMRT->Create(arrRTTex, 2, nullptr);
	}
}

void CRenderManager::ClearMRT()
{
	// Swap Chain 은 모두 Clear
	m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->ClearTarget();
	m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->ClearDepthStencil();

	// Deferred 는 Render Target 만 Clear
	m_MRT[(UINT)MRT_TYPE::DEFERRED]->ClearTarget();
	m_MRT[(UINT)MRT_TYPE::LIGHT]->ClearTarget();
}