#pragma once
#include "CAsset.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshData :
	public CAsset
{
private:
	Ptr<CMesh>				m_pMesh;
	vector<Ptr<CMaterial>>	m_vecMtrl;

public:
	static CMeshData* LoadFromFBX(const wstring& _RelativePath);

	CGameObject* Instantiate();

	void SetMesh(Ptr<CMesh> mesh) { m_pMesh = mesh; }
	void SetMaterial(Ptr<CMaterial> mtrl) { m_vecMtrl.push_back(mtrl); }

	Ptr<CMaterial> GetMaterial(UINT idx) { return m_vecMtrl[idx]; }

public:
	// 파일로 저장
	virtual int Save(const wstring& _strFilePath) override;

	// 파일로부터 로딩
	virtual int Load(const wstring& _strFilePath) override;

public:
	CLONE_DISABLE(CMeshData);

public:
	CMeshData(bool _Engine = false);
	~CMeshData();
};