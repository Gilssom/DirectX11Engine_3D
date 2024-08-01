#pragma once
#include "CRenderComponent.h"

#include "CHeightMapCS.h"
#include "CRaycastCS.h"
#include "CWeightMapCS.h"

class CStructuredBuffer;

struct tRaycastOut
{
	Vec2	Location;
	UINT	Distance;
	int		Success;
};

struct tWeight
{
	float   Weight[8];
};

enum LANDSCAPE_MODE
{
	NONE,
	HEIGHT_MAP,
	SPLATING,
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
	Ptr<CTexture>			m_ColorTex;
	Ptr<CTexture>			m_NormalTex;
	CStructuredBuffer*		m_WeightMap;
	UINT                    m_WeightWidth;
	UINT                    m_WeightHeight;
	Ptr<CWeightMapCS>       m_WeightMapCS;
	int                     m_WeightIdx;


	// Raycasting
	Ptr<CRaycastCS>			m_RaycastCS;
	CStructuredBuffer*		m_RaycastOut;
	tRaycastOut				m_Out;


	// LandScape Mod
	LANDSCAPE_MODE			m_Mode;


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
	void CreateTexture();
	void Binding();
	int Raycasting();

public:
	CLONE(CLandScape);

public:
	CLandScape();
	~CLandScape();
};