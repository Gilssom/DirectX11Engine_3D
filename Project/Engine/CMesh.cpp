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

	// ���� Buffer ����
	m_VBdesc.ByteWidth = sizeof(Vtx) * m_VtxCount;
	m_VBdesc.MiscFlags = 0;

	// Buffer �� �뵵�� ����
	m_VBdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// ���۰� ������ ���Ŀ� CPU ���� �����ؼ� GPU �� �ִ� �����͸�
	// ����Ⱑ �����ϰ� ����
	m_VBdesc.CPUAccessFlags = 0;
	m_VBdesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA Subdesc = {};
	Subdesc.pSysMem = VtxSysMem;

	// define.h -> #define
	if (FAILED(DEVICE->CreateBuffer(&m_VBdesc, &Subdesc, m_VB.GetAddressOf())))
	{
		return E_FAIL;
	}


	// Index Buffer ����
	m_IBdesc.ByteWidth = sizeof(UINT) * m_IdxCount;
	m_IBdesc.MiscFlags = 0;

	// ���� �뵵 ����
	m_IBdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// �ѹ� ������ ���Ŀ� �б�, ���� �Ұ���
	m_IBdesc.CPUAccessFlags = 0;
	m_IBdesc.Usage = D3D11_USAGE_DEFAULT;

	Subdesc = {};
	Subdesc.pSysMem = IdxSysMem;

	if (FAILED(DEVICE->CreateBuffer(&m_IBdesc, &Subdesc, m_IB.GetAddressOf())))
	{
		return E_FAIL;
	}


	// System Memory ����
	m_VtxSysMem = new Vtx[m_VtxCount];
	memcpy(m_VtxSysMem, VtxSysMem, sizeof(Vtx) * m_VtxCount);

	m_IdxSysMem = new UINT[m_IdxCount];
	memcpy(m_IdxSysMem, IdxSysMem, sizeof(UINT) * m_IdxCount);


	return S_OK;
}

void CMesh::Binding()
{
	UINT stride = sizeof(Vtx); // �� ���� �� ����
	UINT offset = 0; // �������� ���� �� ���� ��ġ
	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &stride, &offset);

	// �簢���� �׸� �� �Ȱ��� ������ ������ ���� ���� �� �ۿ� ���µ�
	// �� �κ��� �޸𸮸� ��� ���� �� �ִ�.
	// �̰� �ذ��ϱ� ���ؼ� Index Buffer �� ����ϸ� �ȴ�.
	// Vertex Buffer �� �� 4���� ������ ���� �ǰ�,
	// Index Buffer �� �� 6���� Vertex Buffer �� ����Ű�� ���� ������ �ȴ�.
	CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::Render()
{
	Binding();

	// �簢���� ������ 6�� ( �ﰢ�� �� ���� 3�� )
	//CONTEXT->Draw(6, 0);
	// Index Buffer �� ������ŭ �������� �ؾ� �Ѵ�. ( Vertex Buffer X )
	CONTEXT->DrawIndexed(m_IdxCount, 0, 0);
}

void CMesh::Render_Particle(UINT instance)
{
	Binding();

	// Instance �� ������ ���� ������ ������ ��ü�� �ѹ��� Rendering �� ���� �� �ִ� �Լ�
	// ��, ��� ���� �����Ͱ� ��� �����ؾ��Ѵ�.
	CONTEXT->DrawIndexedInstanced(m_IdxCount, instance, 0, 0, 0);
}
