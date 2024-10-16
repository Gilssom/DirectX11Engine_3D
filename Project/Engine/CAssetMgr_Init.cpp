#include "pch.h"
#include "CAssetManager.h"

#include "CPathManager.h"

#include "CDevice.h"

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
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);

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


	// ===============
	// Cube Mesh
	// ===============
	// 6개의 각 면 마다 각자의 방향벡터(노말, 탄젠트, 바이노말) 이 세팅되어야 하기 때문에
	// 면 단위로 정점 4개씩 6면, 총 24개의 정점이 필요하다.	
	Vtx arrCube[24] = {};

	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUv = Vec2(0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUv = Vec2(0.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUv = Vec2(0.f, 0.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUv = Vec2(0.f, 0.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);


	// 아랫 면	
	arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUv = Vec2(0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUv = Vec2(0.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUv = Vec2(0.f, 0.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUv = Vec2(0.f, 0.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);

	// 왼쪽 면
	arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUv = Vec2(0.f, 0.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUv = Vec2(0.f, 0.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUv = Vec2(0.f, 0.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUv = Vec2(0.f, 0.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUv = Vec2(0.f, 0.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUv = Vec2(0.f, 0.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUv = Vec2(0.f, 0.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUv = Vec2(0.f, 0.f);
	arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);

	// 뒷 면
	arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUv = Vec2(0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUv = Vec2(0.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUv = Vec2(0.f, 0.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUv = Vec2(0.f, 0.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);

	// 앞 면
	arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);;
	arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUv = Vec2(0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUv = Vec2(0.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUv = Vec2(0.f, 0.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUv = Vec2(0.f, 0.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);

	// 인덱스
	for (int i = 0; i < 12; i += 2)
	{
		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 1);
		vecIdx.push_back(i * 2 + 2);

		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 2);
		vecIdx.push_back(i * 2 + 3);
	}

	pMesh = new CMesh(true);
	pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CubeMesh", pMesh);
	vecIdx.clear();

	UINT arrCubeIdx[] = { 0,1,2,3,0,7,6,1,2,5,4,3,0,7,4,5,6 };

	pMesh = new CMesh(true);
	pMesh->Create(arrCube, 24, arrCubeIdx, sizeof(arrCubeIdx) / sizeof(UINT));
	AddAsset(L"CubeMesh_Debug", pMesh);
	vecIdx.clear();


	// ===============
	// Sphere Mesh
	// ===============
	float fRadius = 0.5f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUv = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// Body
	UINT iStackCount = 60; // 가로 분할 개수
	UINT iSliceCount = 60; // 세로 분할 개수

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));

			v.vUv = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent.Normalize();

			v.vNormal.Cross(v.vTangent, v.vBinormal);
			v.vBinormal.Normalize();

			vecVtx.push_back(v);
		}
	}

	// Bottom
	v.vPos = Vec3(0.f, -fRadius, 0.f);
	v.vUv = Vec2(0.5f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();

	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// 인덱스
	// 북극점
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 몸통
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	// 남극점
	UINT iBottomIdx = (UINT)vecVtx.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"SphereMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();



	// ===============
	// Cone Mesh
	// ===============
	fRadius = 0.5f;
	float fHeight = 1.f;
	iStackCount = 20;  // 원뿔의 층 개수
	iSliceCount = 60;  // 원뿔의 슬라이스 개수

	// Bottom center
	v.vPos = Vec3(0.f, -fHeight, 0.f);  // 이동된 위치
	v.vUv = Vec2(0.5f, 0.5f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);  // 밑면 노멀 벡터는 아래쪽을 향함
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	fSliceAngle = XM_2PI / iSliceCount;
	float fStackHeight = fHeight / iStackCount;

	// Bottom vertices
	for (UINT i = 0; i <= iSliceCount; ++i)
	{
		float theta = i * fSliceAngle;

		v.vPos = Vec3(fRadius * cosf(theta), -fHeight, fRadius * sinf(theta));  // 이동된 위치
		v.vUv = Vec2((cosf(theta) + 1.f) * 0.5f, (sinf(theta) + 1.f) * 0.5f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

		Vec3 normal(cosf(theta), fRadius / fHeight, sinf(theta));
		normal.Normalize();
		v.vNormal = normal;

		v.vTangent = Vec3(-sinf(theta), 0.f, cosf(theta));
		v.vBinormal = Vec3(0.f, 1.f, 0.f);
		vecVtx.push_back(v);
	}

	// Side vertices
	for (UINT i = 1; i <= iStackCount; ++i)
	{
		float y = i * fStackHeight - fHeight;  // 이동된 위치
		float r = fRadius * (1.0f - (float)i / iStackCount);

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(r * cosf(theta), y, r * sinf(theta));  // 이동된 위치
			v.vUv = Vec2((float)j / iSliceCount, (float)i / iStackCount);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

			// 옆면 노멀 벡터는 원뿔의 축을 기준으로 반지름 방향으로 설정
			Vec3 normal(cosf(theta), fRadius / fHeight, sinf(theta));
			normal.Normalize();
			v.vNormal = normal;

			v.vTangent = Vec3(-sinf(theta), 0.f, cosf(theta));
			v.vBinormal = Vec3(0.f, 1.f, 0.f);
			vecVtx.push_back(v);
		}
	}

	// Top vertex
	v.vPos = Vec3(0.f, 0.f, 0.f);  // 꼭짓점이 원점
	v.vUv = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// Bottom indices
	for (UINT i = 1; i <= iSliceCount; ++i)
	{
		vecIdx.push_back(0);           // 밑면 중심점의 인덱스
		vecIdx.push_back(i);           // 현재 슬라이스의 정점 인덱스
		vecIdx.push_back(i + 1);       // 다음 슬라이스의 정점 인덱스
	}

	// Side indices
	for (UINT i = 0; i < iStackCount; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			vecIdx.push_back((iSliceCount + 1) * i + j + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + j + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);

			vecIdx.push_back((iSliceCount + 1) * i + j + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * i + (j + 1) + 1);
		}
	}

	// Top indices
	UINT iTopIdx = (UINT)vecVtx.size() - 1;

	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iTopIdx);
		vecIdx.push_back(iTopIdx - (i + 1) - 1);
		vecIdx.push_back(iTopIdx - (i + 2) - 1);
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"ConeMesh", pMesh);
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


	//// ================================
	////  PostProcess Bright Pass Shader
	//// ================================
	//pShader = new CGraphicShader;
	//pShader->CreateVertexShader(strPath + L"shader\\postprocess.fx", "VS_PostProcess");
	//pShader->CreatePixelShader(strPath + L"shader\\postprocess.fx", "PS_BrightPass");

	//pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	//pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	//pShader->AddTexParam("Copy Render Target", TEX_0);
	//pShader->AddTexParam("Emissive Target Texture", TEX_1);
	//pShader->AddTexParam("Output Texture", TEX_2);

	//AddAsset<CGraphicShader>(L"BrightPassShader", pShader);


	// ===========================
	//  PostProcess Bloom Shader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\postprocess.fx", "VS_PostProcess");
	pShader->CreatePixelShader(strPath + L"shader\\postprocess.fx", "PS_Bloom");

	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	pShader->AddTexParam("Copy Render Target", TEX_0);
	pShader->AddTexParam("Bright Pass Texture", TEX_1);

	pShader->AddScalarParam("Bloom Power", FLOAT_0);
	pShader->AddScalarParam("Blur Radius", FLOAT_1);
	pShader->AddScalarParam("Brightness Scale", FLOAT_2);
	pShader->AddScalarParam("Bloom Threshold", FLOAT_3);

	AddAsset<CGraphicShader>(L"BloomShader", pShader);

	// ===========================
	//  PostProcess Bloom Shader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\postprocess.fx", "VS_PostProcess");
	pShader->CreatePixelShader(strPath + L"shader\\postprocess.fx", "PS_Vignette");

	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	pShader->AddTexParam("Copy Render Target", TEX_0);

	pShader->AddScalarParam("Radius", FLOAT_0);
	pShader->AddScalarParam("Sofeness", FLOAT_1);

	AddAsset<CGraphicShader>(L"VignetteShader", pShader);


	// ===========================
	//  PostProcess DOF Shader
	// ===========================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\postprocess.fx", "VS_PostProcess");
	pShader->CreatePixelShader(strPath + L"shader\\postprocess.fx", "PS_DepthOfField");

	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	pShader->AddTexParam("Copy Render Target", TEX_0);
	pShader->AddTexParam("Depth Stencil Target", TEX_1);

	pShader->AddScalarParam("Focus Distance", FLOAT_0);
	pShader->AddScalarParam("Focus Range", FLOAT_1);

	AddAsset<CGraphicShader>(L"DepthOfFieldShader", pShader);


	// ====================
	//	Debug Shape Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\debug_shape.fx", "VS_DebugShape");
	pShader->CreatePixelShader(strPath + L"shader\\debug_shape.fx", "PS_DebugShape");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	AddAsset<CGraphicShader>(L"DebugShapeShader", pShader);


	// ====================
	//	Std3D Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\Std3d.fx", "VS_Std3D");
	pShader->CreatePixelShader(strPath + L"shader\\Std3d.fx", "PS_Std3D");

	pShader->SetRSType(RS_TYPE::CULL_BACK); // 3D 의 기본 Cull Mode
	//pShader->SetRSType(RS_TYPE::WIRE_FRAME); // 3D Test Wire Mode
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	pShader->AddTexParam("Output Texture", TEX_0);
	pShader->AddTexParam("Normal Texture", TEX_1);

	AddAsset<CGraphicShader>(L"Std3DShader", pShader);


	// ====================
	//	SkyBox Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\skybox.fx", "VS_SkyBox");
	pShader->CreatePixelShader(strPath + L"shader\\skybox.fx", "PS_SkyBox");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	pShader->AddTexParam("SkyBox Texture", TEX_0);

	AddAsset<CGraphicShader>(L"SkyBoxShader", pShader);


	// ====================
	//	Deferred Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\std3d_deferred.fx", "VS_Std3D_Deferred");
	pShader->CreatePixelShader(strPath + L"shader\\std3d_deferred.fx", "PS_Std3D_Deferred");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);
	
	pShader->AddTexParam("Color"	, TEX_0);
	pShader->AddTexParam("Normal"	, TEX_1);
	pShader->AddTexParam("Specular"	, TEX_2);
	pShader->AddTexParam("Heightmap", TEX_3);
	pShader->AddTexParam("Emissive"	, TEX_4);

	AddAsset<CGraphicShader>(L"Std3DDeferredShader", pShader);


	// ====================
	//	Dir Light Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\light.fx", "VS_DirLight");
	pShader->CreatePixelShader(strPath + L"shader\\light.fx", "PS_DirLight");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::ONE_ONE);	// 빛이 누적되어서 합쳐져야한다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHTING);

	// Parameter 는 내부에서 직접 지정하기 때문에 외부에 노출될 이유가 없다.

	AddAsset<CGraphicShader>(L"DirLightShader", pShader);


	// ====================
	//	Point Light Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\light.fx", "VS_PointLight");
	pShader->CreatePixelShader(strPath + L"shader\\light.fx", "PS_PointLight");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::ONE_ONE);	// 빛이 누적되어서 합쳐져야한다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHTING);

	// Parameter 는 내부에서 직접 지정하기 때문에 외부에 노출될 이유가 없다.

	AddAsset<CGraphicShader>(L"PointLightShader", pShader);



	// ====================
	//	Spot Light Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\light.fx", "VS_PointLight");
	pShader->CreatePixelShader(strPath + L"shader\\light.fx", "PS_PointLight");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::ONE_ONE);	// 빛이 누적되어서 합쳐져야한다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHTING);

	// Parameter 는 내부에서 직접 지정하기 때문에 외부에 노출될 이유가 없다.

	AddAsset<CGraphicShader>(L"SpotLightShader", pShader);


	// ====================
	//	Merge Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\merge.fx", "VS_Merge");
	pShader->CreatePixelShader(strPath + L"shader\\merge.fx", "PS_Merge");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::DEFAULT);	
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHTING);

	AddAsset<CGraphicShader>(L"MergeShader", pShader);


	// ====================
	//	ShadowMap Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\shadowmap.fx", "VS_ShadowMap");
	pShader->CreatePixelShader(strPath + L"shader\\shadowmap.fx", "PS_ShadowMap");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_SHADOWMAP);

	AddAsset<CGraphicShader>(L"ShadowMapShader", pShader);


	// ====================
	//	Decal Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\decal.fx", "VS_Decal");
	pShader->CreatePixelShader(strPath + L"shader\\decal.fx", "PS_Decal");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	//pShader->SetBSType(BS_TYPE::DECAL_BLEND);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DECAL);

	AddAsset<CGraphicShader>(L"DecalShader", pShader);


	// ====================
	//	Tess Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\tess.fx", "VS_Tess");
	pShader->CreatePixelShader(strPath + L"shader\\tess.fx", "PS_Tess");
	pShader->CreateHullShader(strPath + L"shader\\tess.fx", "HS_Tess");
	pShader->CreateDomainShader(strPath + L"shader\\tess.fx", "DS_Tess");

	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	AddAsset<CGraphicShader>(L"TessShader", pShader);


	// ====================
	//	LandScape Shader
	// ====================
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(strPath + L"shader\\landscape.fx", "VS_LandScape");
	pShader->CreatePixelShader(strPath + L"shader\\landscape.fx", "PS_LandScape");
	pShader->CreateHullShader(strPath + L"shader\\landscape.fx", "HS_LandScape");
	pShader->CreateDomainShader(strPath + L"shader\\landscape.fx", "DS_LandScape");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	AddAsset<CGraphicShader>(L"LandScapeShader", pShader);
}

#include "CSetColorCS.h"
#include "CParticleTickCS.h"
#include "CAnimation3DShader.h"

void CAssetManager::CreateDefaultComputeShader()
{
	Ptr<CComputeShader> pComputeShader = nullptr;

	// Set Color
	pComputeShader = new CSetColorCS;
	// 등록 및 검색은 부모 클래스인 CComputeShader 로
	AddAsset<CComputeShader>(L"SetColorCS", pComputeShader);


	// Particle Tick
	pComputeShader = new CParticleTickCS;
	AddAsset<CComputeShader>(L"ParticleTickCS", pComputeShader);


	// Animation3D
	pComputeShader = new CAnimation3DShader;
	AddAsset<CComputeShader>(L"Animation3DUpdateCS", pComputeShader);
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

	// Bloom Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"BloomMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"BloomShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Vignettes Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"VignetteMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"VignetteShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// DepthOfFieldShader Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"DepthOfFieldMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"DepthOfFieldShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Debug Shape Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"DebugShapeMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"DebugShapeShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Std 3D Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"Std3DMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"Std3DShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// SkyBox Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"SkyBoxMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"SkyBoxShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Deferred Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"Std3DDeferredMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"Std3DDeferredShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Dir Ligth Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"DirLightMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"DirLightShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Point Light Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"PointLightMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"PointLightShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Spot Light Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"SpotLightMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"SpotLightShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Merge Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"MergeMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"MergeShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// ShadowMapMtrl
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"ShadowMapMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"ShadowMapShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Decal Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"DecalMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"DecalShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// Tess Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"TessMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"TessShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);

	// LandScape Material
	pMaterial = new CMaterial(true);
	pMaterial->SetName(L"LandScapeMaterial");
	pMaterial->SetShader(FindAsset<CGraphicShader>(L"LandScapeShader"));
	AddAsset<CMaterial>(pMaterial->GetName(), pMaterial);
}