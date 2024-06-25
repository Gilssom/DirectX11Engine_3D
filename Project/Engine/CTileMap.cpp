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

	// 복사 생성자를 구현 함으로써 해당 생성자를 사용하여 원본 Buffer 를 복제
	// (딱히 GPU 간의 연산을 할 필요가 없기 때문에 효율성을 위해 위의 코드를 사용)
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
	// 위치 정보
	Transform()->Binding();

	// 재질 정보
	GetMaterial()->SetTexParam(TEX_0, m_Atlas);
	GetMaterial()->SetScalarParam(VEC2_0, m_AtlasResolution);
	GetMaterial()->SetScalarParam(VEC2_1, m_AtlasTileEachSize);
	GetMaterial()->SetScalarParam(VEC2_2, Vec2(m_Col, m_Row));
	GetMaterial()->SetScalarParam(INT_0, m_AtlasMaxRow);
	GetMaterial()->SetScalarParam(INT_1, m_AtlasMaxCol);
	GetMaterial()->SetScalarParam(INT_2, 0);
	GetMaterial()->SetScalarParam(INT_3, 0); // Capture Mode 여부
	GetMaterial()->Binding();

	// Tile Info Binding
	if (m_TileBuffer->GetElementCount() < m_vecTileInfo.size())
		m_TileBuffer->Create(sizeof(tTileInfo), m_vecTileInfo.size(), SB_TYPE::SRV_ONLY, true, m_vecTileInfo.data());
	else
		m_TileBuffer->SetData(m_vecTileInfo.data(), m_vecTileInfo.size());

	m_TileBuffer->Binding(20);

	// Mesh Binding 및 Rendering
	GetMesh()->Render();

	// Editor 용 Capture Image Rendering
	if (m_EditorCapture)
	{
		CaptureRender();
	}
}

void CTileMap::CaptureRender()
{
	// 1. 현재 타일의 상황을 캡쳐할 Texture(그림을 그릴 목적지, Render Target) 를 선체크,
	CheckCaptureTexture();

	// 2. Render Target 이 일시적으로 교체되어야 한다.
	// 기존에 세팅되어있던 Target 이 누군지 먼저 알아낸다.
	ComPtr<ID3D11RenderTargetView> pRTV = nullptr;
	ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
	CONTEXT->OMGetRenderTargets(1, pRTV.GetAddressOf(), pDSV.GetAddressOf());

	// 2-2. Capture Texture 로 Render Target 을 변경한다.
	CONTEXT->OMSetRenderTargets(1, m_CaptureTex->GetRTV().GetAddressOf(), nullptr);

	// View Port 설정
	// Texture 크기 영역을 설정한다.
	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (float)m_CaptureTex->GetWidth();
	viewPort.Height = (float)m_CaptureTex->GetHeight();
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	// ViewPort 정보 세팅
	CONTEXT->RSSetViewports(1, &viewPort);

	// 3. Tile 을 교체된 Render Target 에 Rendering 을 한다.
	// 좌표변환 같은 과정이 필요 없다.
	GetMaterial()->SetScalarParam(INT_3, 1); // Capture Mode 체크
	GetMaterial()->Binding();
	GetMesh()->Render();


	// 4. Rendering 이 끝난 후에는 Reder Target 을 원상 복구 시켜야 한다.
	CONTEXT->OMSetRenderTargets(1, pRTV.GetAddressOf(), pDSV.Get());

	// View Port 재설정
	// 윈도우 화면에 보여질 영역을 설정한다.
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (float)CDevice::GetInst()->GetRenderResolution().x;
	viewPort.Height = (float)CDevice::GetInst()->GetRenderResolution().y;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	// ViewPort 정보 재세팅
	CONTEXT->RSSetViewports(1, &viewPort);
}

void CTileMap::CheckCaptureTexture()
{
	// 1. Texture 가 존재하는지 확인 (없으면 생성)
	if (m_CaptureTex == nullptr)
	{
		Vec2 vResolution = Vec2(m_Col * m_TileEachSize.x, m_Row * m_TileEachSize.y);

		// 해상도 제한
		if (vResolution.x > m_CaptureTexMaxResolution.x)
			vResolution.x = m_CaptureTexMaxResolution.x;

		if (vResolution.y > m_CaptureTexMaxResolution.y)
			vResolution.y = m_CaptureTexMaxResolution.y;

		// Texture 새로 생성
		m_CaptureTex = new CTexture(true);

		// Render Target - SRV
		m_CaptureTex->Create((UINT)vResolution.x, (UINT)vResolution.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);
	}
	else
	{
		// Capture Texture 가 생성되어 있지만, 예상되는 해상도 보다 작은 경우 새로 생성.
		Vec2 vResolution = Vec2(m_Col * m_TileEachSize.x, m_Row * m_TileEachSize.y);

		// 해상도 제한
		if (vResolution.x > m_CaptureTexMaxResolution.x)
			vResolution.x = m_CaptureTexMaxResolution.x;

		if (vResolution.y > m_CaptureTexMaxResolution.y)
			vResolution.y = m_CaptureTexMaxResolution.y;

		// Tile Map 의 행과 열이 늘어남
		if (m_CaptureTex->GetWidth() < vResolution.x || m_CaptureTex->GetHeight() < vResolution.y)
		{
			// Texture 새로 생성
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

	// 아틀라스 텍스처 안에서 타일 1개가 차지하는 크기
	m_AtlasTileEachSize = tileSize;

	// 아틀라스 텍스처 해상도를 타일 1개 사이즈로 나누어서 
	// 아틀라스 텍스처에 타일이 몇행 몇열 존재하는지 확인
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