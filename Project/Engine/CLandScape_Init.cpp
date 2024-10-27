#include "pch.h"
#include "CLandScape.h"

#include "CAssetManager.h"
#include "CTexture.h"

#include "CStructuredBuffer.h"

void CLandScape::Init()
{
	// Land Scape ���� Mesh ����
	CreateMesh();

	// Land Scape ���� ���� ����
	Ptr<CMaterial> pMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"LandScapeMaterial");
	SetMaterial(pMaterial, 0);

	// LandScape ���� Compute Shader ����
	CreateComputeShader();

	// LandScape ���� Texture ���� �� �ε�
	CreateTexture();

	// BrushTexture �߰�	
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\TX_GlowScene_2.png"));
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\TX_HitFlash_0.png"));
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\TX_HitFlash02.png"));
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\TX_Twirl02.png"));
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\FX_Flare.png"));

	// Grass Texture ����
	m_GrassTexture = CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\GrassTexture.png");

	m_BrushIdx = 0;
}

void CLandScape::SetFace(UINT x, UINT z)
{
	m_FaceX = x;
	m_FaceZ = z;

	// ����� Face ������ ���� �����ϰ� ����
	CreateMesh();

	// Land Scape ���� ���� ����
	Ptr<CMaterial> pMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"LandScapeMaterial");
	SetMaterial(pMaterial, 0);
}

void CLandScape::CreateComputeShader()
{
	// Height Map CS �� ������ ã�ƿ���, ������ �������ؼ� ����Ѵ�.
	m_HeightMapCS = (CHeightMapCS*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"HeightMapCS").Get();

	if (m_HeightMapCS == nullptr)
	{
		m_HeightMapCS = new CHeightMapCS;
		CAssetManager::GetInst()->AddAsset<CComputeShader>(L"HeightMapCS", m_HeightMapCS.Get());
	}

	// RaycastCS ����
	m_RaycastCS = (CRaycastCS*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"RaycastCS").Get();
	if (nullptr == m_RaycastCS)
	{
		m_RaycastCS = new CRaycastCS;
		CAssetManager::GetInst()->AddAsset<CComputeShader>(L"RaycastCS", m_RaycastCS.Get());
	}

	// WeightMapCS ����
	m_WeightMapCS = (CWeightMapCS*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"WeightMapCS").Get();
	if (nullptr == m_WeightMapCS)
	{
		m_WeightMapCS = new CWeightMapCS;
		CAssetManager::GetInst()->AddAsset<CComputeShader>(L"WeightMapCS", m_WeightMapCS.Get());
	}

	// GrassMapCS ����
	m_GrassMapCS = (CGrassMapCS*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"GrassMapCS").Get();
	if (nullptr == m_GrassMapCS)
	{
		m_GrassMapCS = new CGrassMapCS;
		CAssetManager::GetInst()->AddAsset<CComputeShader>(L"GrassMapCS", m_GrassMapCS.Get());
	}
}

void CLandScape::CreateTexture()
{
	// LandScape ���� Texture Loading
	m_ColorTex = CAssetManager::GetInst()->Load<CTexture>(L"texture\\LandScapeTexture\\LandScape_Color.dds");
	m_ColorTex->GenerateMip(6);

	m_NormalTex = CAssetManager::GetInst()->Load<CTexture>(L"texture\\LandScapeTexture\\LandScape_Normal.dds");
	m_NormalTex->GenerateMip(6);

	// ����ġ Weight Map ���� Structured Buffer
	m_WeightWidth = 2048;
	m_WeightHeight = 2048;

	m_GrassWidth = 2048;
	m_GrassHeight = 2048;

	//tWeight* pWeight = new tWeight[2048 * 2048];
	//
	//for (int i = 0; i < 2048 * 2048; ++i)
	//{
	//	tWeight weight = {};
	//	weight.Weight[1] = 1.f;
	//	pWeight[i] = weight;
	//}

	m_WeightMap->Create(sizeof(tWeight), m_WeightWidth * m_WeightHeight, SB_TYPE::SRV_UAV, true, nullptr);

	m_GrassInstances->Create(sizeof(tGrassInstance), m_GrassWidth * m_GrassHeight, SB_TYPE::SRV_UAV, true, nullptr);

	//delete pWeight;
}

void CLandScape::CreateHeightMap(UINT width, UINT height)
{
	m_IsHeightMapCreated = true;

	m_HeightMap = CAssetManager::GetInst()->CreateTexture(L"LandScapeHeightMap", width, height
													, DXGI_FORMAT_R32_FLOAT
													, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
}

void CLandScape::CreateMesh()
{
	Vtx v;
	vector<Vtx> vecVtx;

	for (int Row = 0; Row < m_FaceZ + 1; ++Row)
	{
		for (int Col = 0; Col < m_FaceX + 1; ++Col)
		{
			v.vPos = Vec3((float)Col, 0.f, (float)Row);
			v.vUv = Vec2((float)Col, (float)m_FaceZ - Row);

			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

			vecVtx.push_back(v);
		}
	}


	vector<UINT> vecIdx;

	for (int Row = 0; Row < m_FaceZ; ++Row)
	{
		for (int Col = 0; Col < m_FaceX; ++Col)
		{
			// 0
			// | \
			// 2--1
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col);
			vecIdx.push_back((m_FaceX + 1) * (Row)+Col + 1);
			vecIdx.push_back((m_FaceX + 1) * (Row)+Col);

			// 1--2
			//  \ |
			//    0
			vecIdx.push_back((m_FaceX + 1) * (Row)+Col + 1);
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col);
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col + 1);
		}
	}

	m_OriginMesh = new CMesh;
	m_OriginMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	SetMesh(m_OriginMesh);
}