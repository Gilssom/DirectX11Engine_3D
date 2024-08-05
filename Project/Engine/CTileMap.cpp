#include "pch.h"
#include "CTileMap.h"

#include "CAssetManager.h"
#include "components.h"

#include "CStructuredBuffer.h"
#include "CTexture.h"

#include "CDevice.h"

CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_Row(1)
	, m_Col(1)
	, m_AtlasMaxRow(0)
	, m_AtlasMaxCol(0)
	, m_TileEachSize(Vec2(32.f, 32.f))
	, m_CaptureTexMaxResolution(Vec2(2048, 2048))
{
	SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"TileMapMaterial"));

	m_TileBuffer = new CStructuredBuffer;

	SetDynamicShadow(false);
}

CTileMap::CTileMap(const CTileMap& other)
	: CRenderComponent(other)
	, m_Row(other.m_Row)
	, m_Col(other.m_Col)
	, m_TileEachSize(other.m_TileEachSize)
	, m_Atlas(other.m_Atlas)
	, m_AtlasMaxRow(other.m_AtlasMaxRow)
	, m_AtlasMaxCol(other.m_AtlasMaxCol)
	, m_AtlasTileEachSize(other.m_AtlasTileEachSize)
	, m_AtlasResolution(other.m_AtlasResolution)
	, m_vecTileInfo(other.m_vecTileInfo)
	, m_TileBuffer(nullptr)
{
	m_TileBuffer = new CStructuredBuffer;
	m_TileBuffer->Create(sizeof(tTileInfo), m_vecTileInfo.size(), SB_TYPE::SRV_ONLY, true, m_vecTileInfo.data());

	// ���� �����ڸ� ���� �����ν� �ش� �����ڸ� ����Ͽ� ���� Buffer �� ����
	// (���� GPU ���� ������ �� �ʿ䰡 ���� ������ ȿ������ ���� ���� �ڵ带 ���)
	//m_TileBuffer = new CStructuredBuffer(*other.m_TileBuffer);
}

CTileMap::~CTileMap()
{
	delete m_TileBuffer;
}

void CTileMap::Begin()
{
	SetRowCol(m_Row, m_Col);
}

void CTileMap::FinalTick()
{

}

void CTileMap::Render()
{
	// ��ġ ����
	Transform()->Binding();

	// ���� ����
	GetMaterial()->SetTexParam(TEX_0, m_Atlas);
	GetMaterial()->SetScalarParam(VEC2_0, m_AtlasResolution);
	GetMaterial()->SetScalarParam(VEC2_1, m_AtlasTileEachSize);
	GetMaterial()->SetScalarParam(VEC2_2, Vec2(m_Col, m_Row));
	GetMaterial()->SetScalarParam(INT_0, m_AtlasMaxRow);
	GetMaterial()->SetScalarParam(INT_1, m_AtlasMaxCol);
	GetMaterial()->SetScalarParam(INT_2, 0);
	GetMaterial()->SetScalarParam(INT_3, 0); // Capture Mode ����
	GetMaterial()->Binding();

	// Tile Info Binding
	if (m_TileBuffer->GetElementCount() < m_vecTileInfo.size())
		m_TileBuffer->Create(sizeof(tTileInfo), m_vecTileInfo.size(), SB_TYPE::SRV_ONLY, true, m_vecTileInfo.data());
	else
		m_TileBuffer->SetData(m_vecTileInfo.data(), m_vecTileInfo.size());

	m_TileBuffer->Binding(20);

	// Mesh Binding �� Rendering
	GetMesh()->Render();

	// Editor �� Capture Image Rendering
	if (m_EditorCapture)
	{
		CaptureRender();
	}
}

void CTileMap::CaptureRender()
{
	// 1. ���� Ÿ���� ��Ȳ�� ĸ���� Texture(�׸��� �׸� ������, Render Target) �� ��üũ,
	CheckCaptureTexture();

	// 2. Render Target �� �Ͻ������� ��ü�Ǿ�� �Ѵ�.
	// ������ ���õǾ��ִ� Target �� ������ ���� �˾Ƴ���.
	ComPtr<ID3D11RenderTargetView> pRTV = nullptr;
	ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
	CONTEXT->OMGetRenderTargets(1, pRTV.GetAddressOf(), pDSV.GetAddressOf());

	// 2-2. Capture Texture �� Render Target �� �����Ѵ�.
	CONTEXT->OMSetRenderTargets(1, m_CaptureTex->GetRTV().GetAddressOf(), nullptr);

	// View Port ����
	// Texture ũ�� ������ �����Ѵ�.
	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (float)m_CaptureTex->GetWidth();
	viewPort.Height = (float)m_CaptureTex->GetHeight();
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	// ViewPort ���� ����
	CONTEXT->RSSetViewports(1, &viewPort);

	// 3. Tile �� ��ü�� Render Target �� Rendering �� �Ѵ�.
	// ��ǥ��ȯ ���� ������ �ʿ� ����.
	GetMaterial()->SetScalarParam(INT_3, 1); // Capture Mode üũ
	GetMaterial()->Binding();
	GetMesh()->Render();


	// 4. Rendering �� ���� �Ŀ��� Reder Target �� ���� ���� ���Ѿ� �Ѵ�.
	CONTEXT->OMSetRenderTargets(1, pRTV.GetAddressOf(), pDSV.Get());

	// View Port �缳��
	// ������ ȭ�鿡 ������ ������ �����Ѵ�.
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (float)CDevice::GetInst()->GetRenderResolution().x;
	viewPort.Height = (float)CDevice::GetInst()->GetRenderResolution().y;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	// ViewPort ���� �缼��
	CONTEXT->RSSetViewports(1, &viewPort);
}

void CTileMap::CheckCaptureTexture()
{
	// 1. Texture �� �����ϴ��� Ȯ�� (������ ����)
	if (m_CaptureTex == nullptr)
	{
		Vec2 vResolution = Vec2(m_Col * m_TileEachSize.x, m_Row * m_TileEachSize.y);

		// �ػ� ����
		if (vResolution.x > m_CaptureTexMaxResolution.x)
			vResolution.x = m_CaptureTexMaxResolution.x;

		if (vResolution.y > m_CaptureTexMaxResolution.y)
			vResolution.y = m_CaptureTexMaxResolution.y;

		// Texture ���� ����
		m_CaptureTex = new CTexture(true);

		// Render Target - SRV
		m_CaptureTex->Create((UINT)vResolution.x, (UINT)vResolution.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);
	}
	else
	{
		// Capture Texture �� �����Ǿ� ������, ����Ǵ� �ػ� ���� ���� ��� ���� ����.
		Vec2 vResolution = Vec2(m_Col * m_TileEachSize.x, m_Row * m_TileEachSize.y);

		// �ػ� ����
		if (vResolution.x > m_CaptureTexMaxResolution.x)
			vResolution.x = m_CaptureTexMaxResolution.x;

		if (vResolution.y > m_CaptureTexMaxResolution.y)
			vResolution.y = m_CaptureTexMaxResolution.y;

		// Tile Map �� ��� ���� �þ
		if (m_CaptureTex->GetWidth() < vResolution.x || m_CaptureTex->GetHeight() < vResolution.y)
		{
			// Texture ���� ����
			m_CaptureTex = new CTexture(true);

			// Render Target - SRV
			m_CaptureTex->Create((UINT)vResolution.x, (UINT)vResolution.y
				, DXGI_FORMAT_R8G8B8A8_UNORM
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);
		}
	}
}

void CTileMap::SetRowCol(UINT row, UINT col)
{
	m_Row = row;
	m_Col = col;

	Transform()->SetRelativeScale(Vec3(m_TileEachSize.x * m_Col, m_TileEachSize.y * m_Row, 1.f));

	m_vecTileInfo.clear();
	m_vecTileInfo.resize(m_Row * m_Col);

	for (int i = 0; i < m_vecTileInfo.size(); i++)
	{
		m_vecTileInfo[i].ImageIndex = i % (m_AtlasMaxRow * m_AtlasMaxCol);
		//m_vecTileInfo[i].ImageIndex = 10;
	}
}

void CTileMap::SetTileEachSize(Vec2 size)
{
	m_TileEachSize = size;

	Transform()->SetRelativeScale(Vec3(m_TileEachSize.x * m_Col, m_TileEachSize.y * m_Row, 1.f));
}

void CTileMap::SetImageIndex(int row, int col, int imageIndex)
{
	if (row < 0 || m_Row <= row || col < 0 || m_Col <= col)
		return;

	int index = row * m_Col + col;
	m_vecTileInfo[index].ImageIndex = imageIndex;
}

void CTileMap::SetAtlasTexture(Ptr<CTexture> tex)
{
	m_Atlas = tex;

	if(m_Atlas != nullptr)
		m_AtlasResolution = Vec2(m_Atlas->GetWidth(), m_Atlas->GetHeight());
}

void CTileMap::SetAtlasTileSize(Vec2 tileSize)
{
	assert(m_Atlas.Get());

	// ��Ʋ�� �ؽ�ó �ȿ��� Ÿ�� 1���� �����ϴ� ũ��
	m_AtlasTileEachSize = tileSize;

	// ��Ʋ�� �ؽ�ó �ػ󵵸� Ÿ�� 1�� ������� ����� 
	// ��Ʋ�� �ؽ�ó�� Ÿ���� ���� � �����ϴ��� Ȯ��
	m_AtlasMaxCol = m_Atlas->GetWidth() / m_AtlasTileEachSize.x;
	m_AtlasMaxRow = m_Atlas->GetHeight() / m_AtlasTileEachSize.y;
}

void CTileMap::SaveToLevelFile(FILE* file)
{
	CRenderComponent::SaveToLevelFile(file);

	fwrite(&m_Row, sizeof(UINT), 1, file);
	fwrite(&m_Col, sizeof(UINT), 1, file);
	fwrite(&m_TileEachSize, sizeof(Vec2), 1, file);

	SaveAssetRef(m_Atlas, file);

	fwrite(&m_AtlasTileEachSize, sizeof(Vec2), 1, file);

	size_t i = m_vecTileInfo.size();
	fwrite(&i, sizeof(size_t), 1, file);

	for (size_t i = 0; i < m_vecTileInfo.size(); i++)
	{
		fwrite(&m_vecTileInfo[i], sizeof(tTileInfo), 1, file);
	}
}

void CTileMap::LoadFromLevelFile(FILE* file)
{
	CRenderComponent::LoadFromLevelFile(file);

	fread(&m_Row, sizeof(UINT), 1, file);
	fread(&m_Col, sizeof(UINT), 1, file);
	fread(&m_TileEachSize, sizeof(Vec2), 1, file);

	LoadAssetRef(m_Atlas, file);

	fread(&m_AtlasTileEachSize, sizeof(Vec2), 1, file);

	size_t vecSize = 0;
	fread(&vecSize, sizeof(size_t), 1, file);

	for (size_t i = 0; i < vecSize; i++)
	{
		tTileInfo info = {};
		fread(&info, sizeof(tTileInfo), 1, file);
		m_vecTileInfo.push_back(info);
	}

	SetAtlasTexture(m_Atlas);
	SetTileEachSize(m_AtlasTileEachSize);
	SetRowCol(m_Row, m_Col);
}