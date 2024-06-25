#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"

CMesh::CMesh(bool bEngine)
	: CAsset(ASSET_TYPE::MESH, bEngine)
	, m_VBdesc{}
	, m_VtxCount(0)
	, m_VtxSysMem(nullptr)
	, m_IBdesc{}
	, m_IdxCount(0)
	, m_IdxSysMem(nullptr)
{

}

CMesh::~CMesh()
{
	if (m_VtxSysMem != nullptr)
		delete[] m_VtxSysMem;

	if (m_IdxSysMem != nullptr)
		delete[] m_IdxSysMem;
}

int CMesh::Create(Vtx* VtxSysMem, size_t VtxCount, UINT* IdxSysMem, size_t IdxCount)
{
	m_VtxCount = (UINT)VtxCount;
	m_IdxCount = (UINT)IdxCount;

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
	m_IBdesc.ByteWidth = sizeof(UINT) * m_IdxCount;
	m_IBdesc.MiscFlags = 0;

	// 버퍼 용도 설정
	m_IBdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// 한번 생성한 이후에 읽기, 쓰기 불가능
	m_IBdesc.CPUAccessFlags = 0;
	m_IBdesc.Usage = D3D11_USAGE_DEFAULT;

	Subdesc = {};
	Subdesc.pSysMem = IdxSysMem;

	if (FAILED(DEVICE->CreateBuffer(&m_IBdesc, &Subdesc, m_IB.GetAddressOf())))
	{
		return E_FAIL;
	}


	// System Memory 유지
	m_VtxSysMem = new Vtx[m_VtxCount];
	memcpy(m_VtxSysMem, VtxSysMem, sizeof(Vtx) * m_VtxCount);

	m_IdxSysMem = new UINT[m_IdxCount];
	memcpy(m_IdxSysMem, IdxSysMem, sizeof(UINT) * m_IdxCount);


	return S_OK;
}

void CMesh::Binding()
{
	UINT stride = sizeof(Vtx); // 각 정점 당 간격
	UINT offset = 0; // 여러개의 정점 중 시작 위치
	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &stride, &offset);

	// 사각형을 그릴 때 똑같은 정점의 정보를 갖고 있을 수 밖에 없는데
	// 이 부분은 메모리를 잡아 먹을 수 있다.
	// 이걸 해결하기 위해선 Index Buffer 를 사용하면 된다.
	// Vertex Buffer 는 총 4개의 정보를 갖게 되고,
	// Index Buffer 는 총 6개의 Vertex Buffer 를 가르키는 값만 가지게 된다.
	CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::Render()
{
	Binding();

	// 사각형은 정점이 6개 ( 삼각형 각 정점 3개 )
	//CONTEXT->Draw(6, 0);
	// Index Buffer 의 갯수만큼 렌더링을 해야 한다. ( Vertex Buffer X )
	CONTEXT->DrawIndexed(m_IdxCount, 0, 0);
}

void CMesh::Render_Particle(UINT instance)
{
	Binding();

	// Instance 의 개수에 따라 정해진 개수의 객체를 한번에 Rendering 을 돌릴 수 있는 함수
	// 단, 모든 세팅 데이터가 모두 동일해야한다.
	CONTEXT->DrawIndexedInstanced(m_IdxCount, instance, 0, 0, 0);
}
