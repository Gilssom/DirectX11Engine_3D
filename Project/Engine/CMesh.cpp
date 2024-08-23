#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"

CMesh::CMesh(bool bEngine)
	: CAsset(ASSET_TYPE::MESH, bEngine)
	, m_VBdesc{}
	, m_VtxCount(0)
	, m_VtxSysMem(nullptr)
{

}

CMesh::~CMesh()
{
	if (m_VtxSysMem != nullptr)
		delete[] m_VtxSysMem;

	for (size_t i = 0; i < m_vecIdxInfo.size(); ++i)
	{
		if (nullptr != m_vecIdxInfo[i].pIdxSysMem)
			delete m_vecIdxInfo[i].pIdxSysMem;
	}
}

CMesh* CMesh::CreateFromContainer(CFBXLoader& loader)
{
	const tContainer* container = &loader.GetContainer(0);

	UINT iVtxCount = (UINT)container->vecPos.size();

	D3D11_BUFFER_DESC tVtxDesc = {};

	tVtxDesc.ByteWidth = sizeof(Vtx) * iVtxCount;
	tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
	if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
		tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
	Vtx* pSys = (Vtx*)tSub.pSysMem;
	for (UINT i = 0; i < iVtxCount; ++i)
	{
		pSys[i].vPos = container->vecPos[i];
		pSys[i].vUv = container->vecUV[i];
		pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
		pSys[i].vNormal = container->vecNormal[i];
		pSys[i].vTangent = container->vecTangent[i];
		pSys[i].vBinormal = container->vecBinormal[i];
		//pSys[i].vWeights = container->vecWeights[i];
		//pSys[i].vIndices = container->vecIndices[i];
	}

	ComPtr<ID3D11Buffer> pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
	{
		return NULL;
	}

	CMesh* pMesh = new CMesh;
	pMesh->m_VB = pVB;
	pMesh->m_VBdesc = tVtxDesc;
	pMesh->m_VtxSysMem = pSys;

	// 인덱스 정보
	UINT iIdxBufferCount = (UINT)container->vecIdx.size();
	D3D11_BUFFER_DESC tIdxDesc = {};

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format 이 R32_UINT 이기 때문
		tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
		if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
			tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		void* pSysMem = malloc(tIdxDesc.ByteWidth);
		memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
		tSub.pSysMem = pSysMem;

		ComPtr<ID3D11Buffer> pIB = nullptr;
		if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
		{
			return NULL;
		}

		tIndexInfo info = {};
		info.tIBDesc = tIdxDesc;
		info.iIdxCount = (UINT)container->vecIdx[i].size();
		info.pIdxSysMem = pSysMem;
		info.pIB = pIB;

		pMesh->m_vecIdxInfo.push_back(info);
	}

	return pMesh;
}

int CMesh::Create(Vtx* VtxSysMem, size_t VtxCount, UINT* IdxSysMem, size_t IdxCount)
{
	m_VtxCount = (UINT)VtxCount;

	// 정점 Buffer 생성
	m_VBdesc.ByteWidth = sizeof(Vtx) * m_VtxCount;
	m_VBdesc.MiscFlags = 0;

	// Buffer 의 용도를 지정
	m_VBdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// 버퍼가 생성된 이후에 CPU 에서 접근해서 GPU 에 있는 데이터를
	// 덮어쓰기가 가능하게 설정
	m_VBdesc.CPUAccessFlags = 0;
	m_VBdesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA Subdesc = {};
	Subdesc.pSysMem = VtxSysMem;

	// define.h -> #define
	if (FAILED(DEVICE->CreateBuffer(&m_VBdesc, &Subdesc, m_VB.GetAddressOf())))
	{
		return E_FAIL;
	}


	// Index Buffer 생성
	tIndexInfo IndexInfo = {};
	IndexInfo.iIdxCount = IdxCount;

	IndexInfo.tIBDesc.ByteWidth = sizeof(UINT) * IdxCount;


	// 버퍼 생성 이후에도, 버퍼의 내용을 수정 할 수 있는 옵션
	IndexInfo.tIBDesc.CPUAccessFlags = 0;
	IndexInfo.tIBDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

	// 정점을 저장하는 목적의 버퍼 임을 알림
	IndexInfo.tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	IndexInfo.tIBDesc.MiscFlags = 0;
	IndexInfo.tIBDesc.StructureByteStride = 0;

	// 초기 데이터를 넘겨주기 위한 정보 구조체	
	Subdesc.pSysMem = IdxSysMem;

	if (FAILED(DEVICE->CreateBuffer(&IndexInfo.tIBDesc, &Subdesc, IndexInfo.pIB.GetAddressOf())))
	{
		assert(nullptr);
	}


	// System Memory 유지
	m_VtxSysMem = new Vtx[m_VtxCount];
	IndexInfo.pIdxSysMem = new UINT[IndexInfo.iIdxCount];

	memcpy(m_VtxSysMem, VtxSysMem, sizeof(Vtx) * m_VtxCount);
	memcpy(IndexInfo.pIdxSysMem, IdxSysMem, sizeof(UINT) * IndexInfo.iIdxCount);

	m_vecIdxInfo.push_back(IndexInfo);

	return S_OK;
}

void CMesh::Binding(UINT iSubset)
{
	UINT iStride = sizeof(Vtx);
	UINT iOffset = 0;

	// 사각형을 그릴 때 똑같은 정점의 정보를 갖고 있을 수 밖에 없는데
	// 이 부분은 메모리를 잡아 먹을 수 있다.
	// 이걸 해결하기 위해선 Index Buffer 를 사용하면 된다.
	// Vertex Buffer 는 총 4개의 정보를 갖게 되고,
	// Index Buffer 는 총 6개의 Vertex Buffer 를 가르키는 값만 가지게 된다.
	//CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);

	// FBX File Mesh Data 추가로 인한 변경
	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::Render(UINT iSubset)
{
	Binding(iSubset);

	// 사각형은 정점이 6개 ( 삼각형 각 정점 3개 )
	//CONTEXT->Draw(6, 0);
	// Index Buffer 의 갯수만큼 렌더링을 해야 한다. ( Vertex Buffer X )
	CONTEXT->DrawIndexed(m_vecIdxInfo[iSubset].iIdxCount, 0, 0);
}

void CMesh::Render_Particle(UINT instance)
{
	Binding(0);

	// Instance 의 개수에 따라 정해진 개수의 객체를 한번에 Rendering 을 돌릴 수 있는 함수
	// 단, 모든 세팅 데이터가 모두 동일해야한다.
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, instance, 0, 0, 0);
}

int CMesh::Save(const wstring& _FilePath)
{
	// 상대경로 저장
	wstring strRelativePath = CPathManager::GetInst()->GetRelativePath(_FilePath);
	SetRelativePath(strRelativePath);

	// 파일 쓰기모드로 열기
	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, _FilePath.c_str(), L"wb");
	assert(pFile);

	// 키값, 상대 경로	
	SaveWString(GetName(), pFile);
	SaveWString(GetKey(), pFile);
	SaveWString(GetRelativePath(), pFile);

	// 정점 데이터 저장				
	int iByteSize = m_VBdesc.ByteWidth;
	fwrite(&iByteSize, sizeof(int), 1, pFile);
	fwrite(m_VtxSysMem, iByteSize, 1, pFile);

	// 인덱스 정보
	UINT iMtrlCount = (UINT)m_vecIdxInfo.size();
	fwrite(&iMtrlCount, sizeof(int), 1, pFile);

	UINT iIdxBuffSize = 0;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
		fwrite(m_vecIdxInfo[i].pIdxSysMem
			, m_vecIdxInfo[i].iIdxCount * sizeof(UINT)
			, 1, pFile);
	}

	fclose(pFile);

	return S_OK;
}

int CMesh::Load(const wstring& _FilePath)
{
	// 읽기모드로 파일열기
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	// 키값, 상대경로
	wstring strName, strKey, strRelativePath;
	LoadWString(strName, pFile);
	LoadWString(strKey, pFile);
	LoadWString(strRelativePath, pFile);

	SetName(strName);
	SetKey(strKey);
	SetRelativePath(strRelativePath);

	// 정점데이터
	UINT iByteSize = 0;
	fread(&iByteSize, sizeof(int), 1, pFile);

	m_VtxSysMem = (Vtx*)malloc(iByteSize);
	fread(m_VtxSysMem, 1, iByteSize, pFile);


	D3D11_BUFFER_DESC tDesc = {};
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.ByteWidth = iByteSize;
	tDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem = m_VtxSysMem;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tSubData, m_VB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// 인덱스 정보
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		tIndexInfo info = {};
		fread(&info, sizeof(tIndexInfo), 1, pFile);

		UINT iByteWidth = info.iIdxCount * sizeof(UINT);

		void* pSysMem = malloc(iByteWidth);
		info.pIdxSysMem = pSysMem;
		fread(info.pIdxSysMem, iByteWidth, 1, pFile);

		tSubData.pSysMem = info.pIdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&info.tIBDesc, &tSubData, info.pIB.GetAddressOf())))
		{
			assert(nullptr);
		}

		m_vecIdxInfo.push_back(info);
	}

	fclose(pFile);

	return S_OK;
}