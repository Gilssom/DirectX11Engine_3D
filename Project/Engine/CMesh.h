#pragma once
#include "CAsset.h"

#include "CFBXLoader.h"

struct tIndexInfo
{
	ComPtr<ID3D11Buffer>    pIB;
	D3D11_BUFFER_DESC       tIBDesc;
	UINT				    iIdxCount;
	void* pIdxSysMem;
};

class CMesh : public CAsset
{
private:
	// ���� ������ �����ϴ� Buffer
	ComPtr<ID3D11Buffer> m_VB;
	D3D11_BUFFER_DESC	 m_VBdesc;
	UINT				 m_VtxCount;
	Vtx*				 m_VtxSysMem;

	// Vertex Buffer ������ ����� ������ ����Ű�� �ε��� ������ �����ϴ� Buffer
	//ComPtr<ID3D11Buffer> m_IB;
	//D3D11_BUFFER_DESC	 m_IBdesc;
	//UINT				 m_IdxCount;
	//UINT*				 m_IdxSysMem;

	// �ϳ��� ���ؽ����ۿ� �������� �ε������۰� ����
	vector<tIndexInfo>		m_vecIdxInfo;

public:
	UINT GetVertexCount() { return m_VtxCount; }
	//UINT GetIndexCount() { return m_IdxCount; }
	UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }

public:
	static CMesh* CreateFromContainer(CFBXLoader& loader);
	int Create(Vtx* VtxSysMem, size_t VtxCount, UINT* IdxSysMem, size_t IdxCount);
	void Render(UINT iSubset);
	void Render_Particle(UINT instance);

private:
	void Binding(UINT _Subset);

public:
	virtual int Load(const wstring& _FilePath) override;
	virtual int Save(const wstring& _FilePath) override;

public:
	CLONE_DISABLE(CMesh);

public:
	CMesh(bool bEngine = false);
	~CMesh();
};

