#include "pch.h"
#include "CLandScape.h"

#include "CStructuredBuffer.h"

#include "CRenderManager.h"
#include "CAssetManager.h"
#include "CLevelManager.h"
#include "CLevel.h"
#include "CTimeManager.h"
#include "CKeyManager.h"

#include "CTransform.h"
#include "CCamera.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "CMeshData.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
	, m_MinLevel(0)
	, m_MaxLevel(4)
	, m_MaxLevelRange(2000.f)
	, m_MinLevelRange(6000.f)
	, m_BrushIdx(-1)
	, m_BrushScale(Vec2(0.2f, 0.2f))
	, m_IsHeightMapCreated(false)
	, m_WeightMap(nullptr)
	, m_WeightWidth(0)
	, m_WeightHeight(0)
	, m_GrassWidth(0)
	, m_GrassHeight(0)
	, m_GrassCount(100)
	, m_WeightIdx(0)
	, m_Mode(LANDSCAPE_MODE::SPLATING)
	, m_IsWireFrameMode(false)
{
	// 가중치 맵
	m_WeightMap = new CStructuredBuffer;
	m_GrassInstances = new CStructuredBuffer;

	Init();
	SetFrustumCheck(false);

	// Raycasting 결과를 받는 용도의 구조화버퍼
	m_RaycastOut = new CStructuredBuffer;
	m_RaycastOut->Create(sizeof(tRaycastOut), 1, SB_TYPE::SRV_UAV, true);
}

CLandScape::~CLandScape()
{
	delete m_RaycastOut;
	delete m_WeightMap;
	delete m_GrassInstances;
}

void CLandScape::FinalTick()
{
	// Mode Change
	if (KEY_TAP(KEY::_6))
	{
		if (m_Mode == HEIGHT_MAP)
			m_Mode = SPLATING;
		else if (m_Mode == SPLATING)
			m_Mode = GRASS_MAP;
		else if (m_Mode == GRASS_MAP)
			m_Mode = NONE;
		else
			m_Mode = HEIGHT_MAP;
	}

	// Brush Change
	if (KEY_TAP(KEY::_7))
	{
		++m_BrushIdx;

		if (m_vecBrush.size() <= m_BrushIdx)
			m_BrushIdx = 0;
	}

	// Weight Map Change
	if (KEY_TAP(KEY::_8))
	{
		++m_WeightIdx;

		if (m_ColorTex->GetArraySize() <= m_WeightIdx)
			m_WeightIdx = 0;
	}

	if (m_Mode == NONE)
		return;

	Raycasting();

	if (m_Mode == HEIGHT_MAP)
	{
		if (m_IsHeightMapCreated && KEY_PRESSED(KEY::LBTN))
		{
			// RayCasting
			if (m_Out.Success)
			{
				// 높이맵 설정
				m_HeightMapCS->SetBrushPos(m_RaycastOut);
				m_HeightMapCS->SetBrushScale(m_BrushScale);
				m_HeightMapCS->SetHeightMap(m_HeightMap);
				m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
				m_HeightMapCS->Execute();
			}
		}
	}
	else if (m_Mode == SPLATING)
	{
		if (KEY_PRESSED(KEY::LBTN) && m_WeightWidth != 0 && m_WeightHeight != 0)
		{
			m_WeightMapCS->SetBrushPos(m_RaycastOut);
			m_WeightMapCS->SetBrushScale(m_BrushScale);
			m_WeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
			m_WeightMapCS->SetWeightMap(m_WeightMap);
			m_WeightMapCS->SetWeightIdx(m_WeightIdx);
			m_WeightMapCS->SetWeightMapWidthHeight(m_WeightWidth, m_WeightHeight);

			m_WeightMapCS->Execute();
		}
	}
	else if (m_Mode == GRASS_MAP)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			// RayCasting
			if (m_Out.Success)
			{
				Vec3 vScale = Transform()->GetRelativeScale();
				Vec3 hitPointLocal = Vec3(m_Out.Location.x * (m_FaceX * vScale.x), 0.0f, (1.0f - m_Out.Location.y) * (m_FaceZ * vScale.z));
				Vec2 brushScale = Vec2(m_BrushScale.x * m_WeightWidth, m_BrushScale.y * m_WeightHeight);

				int grassCount = 10; // 임시 Test
				float brushRadiusX = brushScale.x * 0.5f;  // brush 영역의 반지름 (X축)
				float brushRadiusZ = brushScale.y * 0.5f;  // brush 영역의 반지름 (Z축)

				// Create Grass
				CGameObject* pGrassTest = nullptr;
				Ptr<CMeshData> pGrassMeshData = nullptr;
				pGrassMeshData = CAssetManager::GetInst()->FindAsset<CMeshData>(L"meshdata\\Grass_2.mdat");
				pGrassMeshData->GetMaterial(0)->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\GrassTexture_2.png"));
				
				for (int i = 0; i < grassCount; i++)
				{
					pGrassTest = pGrassMeshData->Instantiate();

					// 풀의 랜덤 위치를 hitPointLocal 기준으로 설정
					float randomOffsetX = GetRandomFloat(-brushScale.x * 0.5f, brushScale.x * 0.5f);
					float randomOffsetZ = GetRandomFloat(-brushScale.y * 0.5f, brushScale.y * 0.5f);
					Vec3 randomPos = hitPointLocal + Vec3(randomOffsetX, 0.0f, randomOffsetZ);

					// 풀의 위치와 크기 설정
					pGrassTest->SetName(L"Test");
					pGrassTest->GetRenderComponent()->SetFrustumCheck(false);
					pGrassTest->Transform()->SetRelativePos(randomPos);
					pGrassTest->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

					// 현재 레벨에 추가
					SpawnObject(0, pGrassTest);
				}
			}		
		}
	}
}

void CLandScape::Render()
{
	Binding();

	// WeightMap
	m_WeightMap->Binding(20);

	// GrassMap
	m_GrassInstances->Binding(21);

	if (m_Mode != GRASS_MAP)
	{
		//SetMesh(m_OriginMesh);
		GetMesh()->Render(0);
	}
	else if (m_Mode == GRASS_MAP)
	{
		//SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"PointMesh"));
		GetMesh()->Render_Particle(m_GrassCount);
	}

	// WeightMap
	m_WeightMap->Clear();

	// GrassMap
	m_GrassInstances->Clear();
}

void CLandScape::Binding()
{
	// 위치 정보
	Transform()->Binding();


	// Wire Frame Test Setting
	if(m_IsWireFrameMode)
		GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);
	else
		GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_BACK);


	// 재질 정보 (지형에 대한 데이터 전달)
	CCamera* pCam = CRenderManager::GetInst()->GetFOVCamera();

	GetMaterial(0)->SetScalarParam(INT_0, m_FaceX);
	GetMaterial(0)->SetScalarParam(INT_1, m_FaceZ);
	GetMaterial(0)->SetScalarParam(INT_2, m_Mode);
	GetMaterial(0)->SetScalarParam(INT_3, (int)m_ColorTex->GetArraySize());

	GetMaterial(0)->SetScalarParam(VEC4_0, Vec4(m_MinLevel, m_MaxLevel, m_MinLevelRange, m_MaxLevelRange));
	GetMaterial(0)->SetScalarParam(VEC4_1, pCam->Transform()->GetWorldPos());

	GetMaterial(0)->SetTexParam(TEX_0, m_HeightMap);
	GetMaterial(0)->SetTexParam(TEX_ARR_0, m_ColorTex);
	GetMaterial(0)->SetTexParam(TEX_ARR_1, m_NormalTex);
	GetMaterial(0)->SetTexParam(TEX_2, m_GrassTexture);

	// Brush 정보
	GetMaterial(0)->SetTexParam(TEX_1, m_vecBrush[m_BrushIdx]);
	GetMaterial(0)->SetScalarParam(VEC2_0, m_BrushScale);
	GetMaterial(0)->SetScalarParam(VEC2_1, m_Out.Location);
	GetMaterial(0)->SetScalarParam(VEC2_2, Vec2(m_WeightWidth, m_WeightHeight));
	GetMaterial(0)->SetScalarParam(VEC2_3, Vec2(m_GrassWidth, m_GrassHeight));

	GetMaterial(0)->Binding();
}

int CLandScape::Raycasting()
{
	// 현재 시점 카메라 가져오기
	CCamera* pCam = CRenderManager::GetInst()->GetFOVCamera();
	if (nullptr == pCam)
		return false;

	// 구조화버퍼 클리어
	m_Out = {};
	m_Out.Distance = 0xffffffff;
	m_RaycastOut->SetData(&m_Out, 1);

	// 카메라가 시점에서 마우스를 향하는 Ray 정보를 가져옴
	tRay ray = pCam->GetRay();

	// LandScape 의 WorldInv 행렬 가져옴
	const Matrix& matWorldInv = Transform()->GetWorldMatInv();

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	ray.vDir.Normalize();

	// Raycast Compute Shader에 필요한 데이터 전달
	m_RaycastCS->SetRayInfo(ray);
	m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	m_RaycastCS->SetOutBuffer(m_RaycastOut);
	m_RaycastCS->SetHeightMap(m_HeightMap);

	// Compute Shader 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RaycastOut->GetData(&m_Out, 1);

	return m_Out.Success;
}

void CLandScape::GrassRender()
{

}