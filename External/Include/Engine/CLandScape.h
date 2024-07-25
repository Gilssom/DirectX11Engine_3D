#pragma once
#include "CRenderComponent.h"

#include "CHeightMapCS.h"

class CLandScape : public CRenderComponent
{
private:
	UINT				m_FaceX;
	UINT				m_FaceZ;

	Vec2				m_BrushScale;

	Ptr<CTexture>		m_HeightMap;
	bool				m_IsHeightMapCreated;

	Ptr<CHeightMapCS>	m_HeightMapCS;

public:
	void SetFace(UINT x, UINT z);
	void SetHeightMap(Ptr<CTexture> heightMap) { m_HeightMap = heightMap; m_IsHeightMapCreated = false; }
	void CreateHeightMap(UINT width, UINT height);

public:
	void Init();
	virtual void FinalTick() override;
	virtual void Render() override;

private:
	void CreateMesh();
	void CreateComputeShader();
	void Binding();

public:
	CLONE(CLandScape);

public:
	CLandScape();
	~CLandScape();
};