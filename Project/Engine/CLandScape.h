#pragma once
#include "CRenderComponent.h"

#include "CHeightMapCS.h"
#include "CRaycastCS.h"

class CStructuredBuffer;

struct tRaycastOut
{
	Vec2	Location;
	float	Distance;
	int		Success;
};

class CLandScape : public CRenderComponent
{
private:
	UINT					m_FaceX;
	UINT					m_FaceZ;

	Vec2					m_BrushScale;
	vector<Ptr<CTexture>>	m_vecBrush;
	UINT					m_BrushIdx;

	Ptr<CTexture>			m_HeightMap;
	bool					m_IsHeightMapCreated;
	Ptr<CHeightMapCS>		m_HeightMapCS;

	Ptr<CRaycastCS>			m_RaycastCS;
	CStructuredBuffer*		m_RaycastOut;

public:
	void SetFace(UINT x, UINT z);
	void SetHeightMap(Ptr<CTexture> heightMap) { m_HeightMap = heightMap; m_IsHeightMapCreated = false; }
	void AddBrushTexture(Ptr<CTexture> brushTex) { m_vecBrush.push_back(brushTex); }
	void CreateHeightMap(UINT width, UINT height);

public:
	void Init();
	virtual void FinalTick() override;
	virtual void Render() override;

private:
	void CreateMesh();
	void CreateComputeShader();
	void Binding();
	void Raycasting();

public:
	CLONE(CLandScape);

public:
	CLandScape();
	~CLandScape();
};