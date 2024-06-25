#include "pch.h"
#include "CAssetManager.h"

#include "CPathManager.h"

void CAssetManager::Init()
{
	CreateDefaultMesh();

	CreateDefaultTexture();

	CreateDefaultGraphicShader();

	CreateDefaultComputeShader();

	CreateDefaultMaterial();
}

void CAssetManager::CreateDefaultMesh()
{
	Ptr<CMesh> pMesh = nullptr;

	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	Vtx v;

	// ===============
	// Point Mesh
	// ===============
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUv = Vec2(0.f, 0.f);

	UINT idx = 0;

	pMesh = new CMesh(true);
	pMesh->Create(&v, 1, &idx, 1);
	AddAsset<CMesh>(L"PointMesh", pMesh);


	// ===============
	// Rect Mesh
	// ===============
	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUv = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUv = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUv = Vec2(1.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUv = Vec2(0.f, 1.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(2);
	vecIdx.push_back(3);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	// = 사각형 완성

	// Rect Mesh 생성
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"RectMesh", pMesh);
	vecIdx.clear();

	// Debug Mesh Rect
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"RectMesh_Debug", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	// ===============
	// Circle Mesh
	// ===============
	//vector<Vtx> vecVtx;
	//vector<UINT> vecIdx;
	//Vtx v;

	// 원점을 vector 에 넣는다.
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	float radius = 0.5f;
	UINT slice = 60;
	float angleStep = (2 * XM_PI) / slice; // 한 조각의 각도

	float angle = 0.f;
	for (UINT i = 0; i <= slice; ++i)
	{
		v.vPos = Vec3(cosf(angle) * radius, sinf(angle) * radius, 0.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);

		angle += angleStep;
	}

	for (UINT i = 0; i < slice; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"CircleMesh", pMesh);
	vecIdx.clear();

	// Debug Circle Mesh
	for (int i = 0; i <= slice; ++i)
	{
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"CircleMesh_Debug", pMesh);
	vecVtx.clear();
	vecIdx.clear();
}

void CAssetManager::CreateDefaultTexture()
{
	Load<CTexture>(L"texture\\Character.png", L"texture\\Character.png");
	Load<CTexture>(L"texture\\Fighter.bmp", L"texture\\Fighter.bmp");
}

void CAssetManager::CreateDefaultGraphicShader()
{
	// 경로 찾기 Manager 구현
	wstring strPath = CPathManager::GetInst()->GetContentPath();
	Ptr<CGraphicShader> pShader = nullptr;

	// =======================
	//	Std 2D Default Shader 
	// =======================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(strPath + L"shader\\std2d.fx", "PS_Std2D");

	pShader->SetRSType(RS_TYPE::CULL_NONE); // 2D 에서는 전면 후면 개념이 딱히 필요 없기 때문에 None
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASKED);

	// Parameter Info
	pShader->AddScalarParam("Test Parameter", SCALAR_PARAM::INT_0);
	pShader->AddScalarParam("Change Event", SCALAR_PARAM::INT_1);
	pShader->AddScalarParam("Paper Burn Intence", SCALAR_PARAM::FLOAT_0);
	pShader->AddTexParam("Output", TEX_0);
	pShader->AddTexParam("Spare Texture", TEX_1);
	pShader->AddTexParam("PaperBurn Noise", TEX_2);

	AddAsset<CGraphicShader>(L"Std2DShader", pShader);

	// =======================
	//	Std 2D Alpha Shader 
	// =======================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(strPath + L"shader\\std2d.fx", "PS_Std2D");

	pShader->SetRSType(RS_TYPE::CULL_NONE); // 2D 에서는 전면 후면 개념이 딱히 필요 없기 때문에 None
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASKED);

	// Parameter Info
	pShader->AddScalarParam("Test Parameter", SCALAR_PARAM::INT_0);
	pShader->AddScalarParam("Change Event", SCALAR_PARAM::INT_1);
	pShader->AddTexParam("Output", TEX_0);
	pShader->AddTexParam("Spare Texture", TEX_1);

	AddAsset<CGraphicShader>(L"Std2DAlphaShader", pShader);


	// ===========================
	//	Std 2D Alpha Blend Shader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(strPath + L"shader\\std2d.fx", "PS_Std2D_AB");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	// Parameter Info
	pShader->AddScalarParam("Test Parameter", SCALAR_PARAM::INT_0);
	pShader->AddScalarParam("Change Event", SCALAR_PARAM::INT_1);
	pShader->AddScalarParam("Alpha Check", SCALAR_PARAM::INT_2);
	pShader->AddScalarParam("Alpha Down", SCALAR_PARAM::FLOAT_1);
	pShader->AddTexParam("Output", TEX_0);
	pShader->AddTexParam("Spare Texture", TEX_1);

	AddAsset<CGraphicShader>(L"Std2DAlphaBlendShader", pShader);


	// =================
	//	Tile Map Shader
	// =================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\tilemap.fx", "VS_TileMap");
	pShader->CreatePixelShader(strPath + L"shader\\tilemap.fx", "PS_TileMap");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASKED);

	AddAsset<CGraphicShader>(L"TileMapShader", pShader);


	// ========================
	//  Particle System Shader
	// ========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\particle.fx", "VS_Particle");
	pShader->CreateGeometryShader(strPath + L"shader\\particle.fx", "GS_Particle");
	pShader->CreatePixelShader(strPath + L"shader\\particle.fx", "PS_Particle");

	// Point List 로 Topology 변경 (점 1개짜리를 사용할 것이기 때문에)
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddAsset<CGraphicShader>(L"ParticleRenderShader", pShader);


	// ===========================
	//  PostProcess Filter Shader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\postprocess.fx", "VS_PostProcess");
	pShader->CreatePixelShader(strPath + L"shader\\postprocess.fx", "PS_PostProcess");

	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	pShader->AddTexParam("Copy Render Target", TEX_0);
	pShader->AddTexParam("Noise Texture", TEX_1);

	AddAsset<CGraphicShader>(L"FilterShader", pShader);


	// ==============================
	//  PostProcess Distortion Shader
	// ==============================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\postprocess.fx", "VS_Distortion");
	pShader->CreatePixelShader(strPath + L"shader\\postprocess.fx", "PS_Distortion");

	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	pShader->AddTexParam("Copy Render Target", TEX_0);
	pShader->AddTexParam("Noise Texture", TEX_1);

	AddAsset<CGraphicShader>(L"DistortionShader", pShader);


	// ====================
	//	Debug Shape Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\debug_shape.fx", "VS_DebugShape");
	pShader->CreatePixelShader(strPath + L"shader\\debug_shape.fx", "PS_DebugShape");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	AddAsset<CGraphicShader>(L"DebugShapeShader", pShader);
}

#include "CSetColorCS.h"
#include "CParticleTickCS.h"

void CAssetManager::CreateDefaultComputeShader()
{
	wstring strPath = CPathManager::GetInst()->GetContentPath();
	Ptr<CComputeShader> pComputeShader = nullptr;

	// Set Color
	pComputeShader = new CSetColorCS;
	pComputeShader->CreateComputeShader(strPath + L"shader\\compute.fx", "CS_Test");
	// 등록 및 검색은 부모 클래스인 CComputeShader 로
	AddAsset<CComputeShader>(L"SetColorCS", pComputeShader);


	// Particle Tick
	pComputeShader = new CParticleTickCS;
	pComputeShader->CreateComputeShader(strPath + L"shader\\particletick.fx", "CS_ParticleTick");
	AddAsset<CComputeShader>(L"ParticleTickCS", pComputeShader);
}

void CAssetManager::CreateDefaultMaterial()
{
	Ptr<CMaterial> pMaterial = nullptr;

	// Std 2D Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"Std2DMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"Std2DShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Std 2D Alpha Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"Std2DAlphaMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"Std2DAlphaBlendShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Back Ground Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"BackGroundMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"Std2DAlphaBlendShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Tile Map Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"TileMapMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"TileMapShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Particle Render Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"ParticleMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"ParticleRenderShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// FilterMtrl
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"FilterMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"FilterShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// DistortionMtrl
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"DistortionMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"DistortionShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Debug Shape Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"DebugShapeMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"DebugShapeShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);
}