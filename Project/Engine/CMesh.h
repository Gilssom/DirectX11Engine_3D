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
	// 정점 정보를 저장하는 Buffer
	ComPtr<ID3D11Buffer> m_VB;
	D3D11_BUFFER_DESC	 m_VBdesc;
	UINT				 m_VtxCount;
	Vtx*				 m_VtxSysMem;

	// Vertex Buffer 내에서 사용할 정점을 가르키는 인덱스 정보를 저장하는 Buffer
	//ComPtr<ID3D11Buffer> m_IB;
	//D3D11_BUFFER_DESC	 m_IBdesc;
	//UINT				 m_IdxCount;
	//UINT*				 m_IdxSysMem;

	// 하나의 버텍스버퍼에 여러개의 인덱스버퍼가 연결
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

