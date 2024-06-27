#include "pch.h"
#include "CRenderManager.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CAssetManager.h"

#include "CCamera.h"
#include "CLight2D.h"
#include "CLight3D.h"

#include "CLevelManager.h"

CRenderManager::CRenderManager()
	 : m_EditorCam(nullptr)
	 , m_Light2DBuffer(nullptr)
	 , m_Light3DBuffer(nullptr)
{
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light3DBuffer = new CStructuredBuffer;

	Render_Func = &CRenderManager::Render_Play;
}

CRenderManager::~CRenderManager()
{
	delete m_Light2DBuffer;
	delete m_Light3DBuffer;
}

void CRenderManager::CopyRenderTarget()
{
	Ptr<CTexture> pRenderTargetTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_RenderTargetCopyTex->GetTex2D().Get(), pRenderTargetTex->GetTex2D().Get());
}

void CRenderManager::Init()
{
	// RenderTarget �� ������� �� �ִ� �ؽ��ĸ� ����
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();
	m_RenderTargetCopyTex = CAssetManager::GetInst()->CreateTexture(L"RenderTargetCopyTex"
							, (UINT)vRenderResol.x, (UINT)vRenderResol.y
							, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);
}

void CRenderManager::Tick()
{
}

void CRenderManager::Render()
{
	// Level �� �������� ������ Rendering X
	if (!CLevelManager::GetInst()->GetCurrentLevel())
		return;

	// Swap Chain �� Back Buffer �� Render Target ���� ������
	Ptr<CTexture> pRTTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	Ptr<CTexture> pDSTex = CAssetManager::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
	CONTEXT->OMSetRenderTargets(1, pRTTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());

	// �������� �ʿ��� ������ ���ε�
	DataBinding();

	// Target Clear
	float clearColor[4] = { 0.3f, 0.3f, 0.3f, 1.f };
	CDevice::GetInst()->ClearTarget(clearColor);

	// �Լ� �����ͷ� ���ϴ� �Լ��� ȣ�� ��ų �� �ֵ��� (Render Play or Editor)
	(this->*Render_Func)();

	// ������ Ŭ����
	DataClear();
}

void CRenderManager::Render_Play()
{
	// Camera Draw in Render Target
	for (size_t i = 0; i < m_vecCam.size(); i++)
	{
		if (m_vecCam[i] == nullptr)
			continue;

		m_vecCam[i]->Render();
	}
}

void CRenderManager::Render_Editor()
{
	if(m_EditorCam != nullptr)
		m_EditorCam->Render();
}

void CRenderManager::DataBinding()
{
	// Global Data Binding
	g_GlobalData.Light2DCount = (int)m_vecLight2D.size();
	g_GlobalData.Light3DCount = (int)m_vecLight3D.size();

	static CConstBuffer* GlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	GlobalBuffer->SetData(&g_GlobalData);
	GlobalBuffer->Binding();
	GlobalBuffer->Binding_CS();


	// =========
	// Light 2D
	// =========
	
	// Light ����ȭ ���۷� Binding
	// ũ�Ⱑ �� Ŀ���� ���Ӱ� Create ( ������ ������ ���� ������ )
	// ���� �������� ����ȭ ���� ���ũ�Ⱑ �� ������ Ȯ��.
	if (m_Light2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		m_Light2DBuffer->Create(sizeof(tLightInfo), (UINT)m_vecLight2D.size(), SB_TYPE::SRV_ONLY, true);
	}

	// m_vecLight2D �� ���� ������ ������ ����ȭ ���۷� ����
	static vector<tLightInfo> vecLightInfo;
	vecLightInfo.clear();

	for (size_t i = 0; i < m_vecLight2D.size(); i++)
	{
		vecLightInfo.push_back(m_vecLight2D[i]->GetLightInfo());
	}

	// ������ Level �� 1�� �̻��� �ִٸ�
	if (!vecLightInfo.empty())
	{
		m_Light2DBuffer->SetData(vecLightInfo.data(), m_vecLight2D.size());
		m_Light2DBuffer->Binding(15);
	}



	// =========
	// Light 3D
	// =========
	
	// Light ����ȭ ���۷� Binding
	// ũ�Ⱑ �� Ŀ���� ���Ӱ� Create ( ������ ������ ���� ������ )
	// ���� �������� ����ȭ ���� ���ũ�Ⱑ �� ������ Ȯ��.
	if (m_Light3DBuffer->GetElementCount() < m_vecLight3D.size())
	{
		m_Light3DBuffer->Create(sizeof(tLightInfo), (UINT)m_vecLight3D.size(), SB_TYPE::SRV_ONLY, true);
	}

	// m_vecLight3D �� ���� ������ ������ ����ȭ ���۷� ����
	vecLightInfo.clear();

	for (size_t i = 0; i < m_vecLight3D.size(); i++)
	{
		vecLightInfo.push_back(m_vecLight3D[i]->GetLightInfo());
	}

	// ������ Level �� 1�� �̻��� �ִٸ�
	if (!vecLightInfo.empty())
	{
		m_Light3DBuffer->SetData(vecLightInfo.data(), m_vecLight3D.size());
		m_Light3DBuffer->Binding(16);
	}
}

void CRenderManager::DataClear()
{
	m_vecLight2D.clear();
	m_vecLight3D.clear();
}

void CRenderManager::RegisterCamera(CCamera* newCamera, int priority)
{
	// ī�޶� ��� | Vector Size �� �ʿ信 ���� ��� �ø���
	if (m_vecCam.size() <= priority)
	{
		m_vecCam.resize(priority + 1);
	}

	// �̹� �ش� �ڸ��� �ٸ� Camera �� �ְ�, �ش� Camera �� �ڱ�� �ٸ��ٸ� assert
	if (m_vecCam[priority] && m_vecCam[priority] != newCamera)
		assert(nullptr);

	m_vecCam[priority] = newCamera;
}