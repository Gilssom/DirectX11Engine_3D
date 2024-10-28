#pragma once
#include "CRenderComponent.h"

#include "CHeightMapCS.h"
#include "CRaycastCS.h"
#include "CWeightMapCS.h"
#include "CGrassMapCS.h"

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

struct tGrassInstance
{
	Vec4	Position;		// 풀 인스턴스의 위치
	float	Rotation[3];	// 각 평면의 회전 값
	float	TimeOffset;		// 흔들림 효과를 위한 시간 오프셋
};

enum LANDSCAPE_MODE
{
	NONE,
	HEIGHT_MAP,
	SPLATING,
	GRASS_MAP,
};

class CLandScape : public CRenderComponent
{
private:
	UINT					m_FaceX;
	UINT					m_FaceZ;
	Ptr<CMesh>				m_OriginMesh;


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


	// Grass Map
	Ptr<CGrassMapCS>        m_GrassMapCS;
	CStructuredBuffer*		m_GrassInstances;
	UINT                    m_GrassWidth;
	UINT                    m_GrassHeight;

	vector<Ptr<CTexture>>   m_GrassTexture;
	int						m_GrassTextureIdx;
	UINT                    m_GrassCount;
	Ptr<CMeshData>			m_GrassMeshData;


	// LandScape Mod
	LANDSCAPE_MODE			m_Mode;


	// Wire Mode
	bool					m_IsWireFrameMode;

public:
	void SetFace(UINT x, UINT z);
	void SetHeightMap(Ptr<CTexture> heightMap) { m_HeightMap = heightMap; m_IsHeightMapCreated = false; }
	void CreateHeightMap(UINT width, UINT height);

	Ptr<CTexture> GetWeightMapTexture() { return m_ColorTex; }
	int GetWeightTextureCount() { return m_ColorTex->GetArraySize(); }

	void SetWeightIndex(int weightIdx) { m_WeightIdx = weightIdx; }
	int GetWeightIndex() { return m_WeightIdx; }

	void SetMode(LANDSCAPE_MODE mode) { m_Mode = mode; }
	LANDSCAPE_MODE GetMode() { return m_Mode; }

	void AddBrushTexture(Ptr<CTexture> brushTex) { m_vecBrush.push_back(brushTex); }
	vector<Ptr<CTexture>>& GetBrushTextures() { return m_vecBrush; }

	void SetBrushScale(Vec2 scale) { m_BrushScale = scale; }
	Vec2 GetBrushScale() { return m_BrushScale; }

	void SetBrushIndex(int brushIdx) { m_BrushIdx = brushIdx; }
	int GetBrushIndex() { return m_BrushIdx; }

	void SetMinTessLevel(float minLevel) { m_MinLevel = minLevel; }
	float GetMinTessLevel() { return m_MinLevel; }

	void SetMaxTessLevel(float maxLevel) { m_MaxLevel = maxLevel; }
	float GetMaxTessLevel() { return m_MaxLevel; }

	void SetWireframeMode(bool wireFrame) { m_IsWireFrameMode = wireFrame; }
	bool IsWireframeMode() { return m_IsWireFrameMode; }

	void AddGrassTexture(Ptr<CTexture> grassTex) { m_GrassTexture.push_back(grassTex); }
	vector<Ptr<CTexture>>& GetGrassTextures() { return m_GrassTexture; }

	void SetGrassCount(UINT grassCnt) { m_GrassCount = grassCnt; }
	UINT GetGrassCount() { return m_GrassCount; }

	void SetGrassMeshData(Ptr<CMeshData> meshdata) { m_GrassMeshData = meshdata; }
	Ptr<CMeshData> GetGrassMeshData() { return m_GrassMeshData; }

	void SetSelectedGrassTextureIndex(int grassTexIdx) { m_GrassTextureIdx = grassTexIdx; }
	int GetSelectedGrassTextureIndex() { return m_GrassTextureIdx; }

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
	void GrassRender();

public:
	CLONE(CLandScape);

public:
	CLandScape();
	~CLandScape();
};