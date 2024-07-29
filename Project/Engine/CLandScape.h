#pragma once
#include "CRenderComponent.h"

#include "CHeightMapCS.h"
#include "CRaycastCS.h"

class CStructuredBuffer;

struct tRaycastOut
{
	Vec2	Location;
	UINT	Distance;
	int		Success;
};

class CLandScape : public CRenderComponent
{
private:
	UINT					m_FaceX;
	UINT					m_FaceZ;


	// Brush
	Vec2					m_BrushScale;
	vector<Ptr<CTexture>>	m_vecBrush;
	UINT					m_BrushIdx;


	// Tessellation
	float					m_MinLevel;			// 최소 Tess_Level
	float					m_MaxLevel;			// 최대 Tess_Level
	float					m_MaxLevelRange;	// 최대 Level 거리
	float					m_MinLevelRange;	// 최소 Level 거리


	// Height Map
	Ptr<CTexture>			m_HeightMap;
	bool					m_IsHeightMapCreated;
	Ptr<CHeightMapCS>		m_HeightMapCS;


	// Weight Map


	// Raycasting
	Ptr<CRaycastCS>			m_RaycastCS;
	CStructuredBuffer*		m_RaycastOut;
	tRaycastOut				m_Out;

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
	int Raycasting();

public:
	CLONE(CLandScape);

public:
	CLandScape();
	~CLandScape();
};