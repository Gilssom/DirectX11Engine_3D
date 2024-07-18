#pragma once
#include "CRenderComponent.h"

class CLandScape : public CRenderComponent
{
private:
	UINT			m_FaceX;
	UINT			m_FaceZ;

	Ptr<CTexture>	m_HeightMap;

public:
	void SetFace(UINT x, UINT z);
	void SetHeightMap(Ptr<CTexture> heightMap) { m_HeightMap = heightMap; }

public:
	void Init();
	virtual void FinalTick() override;
	virtual void Render() override;

private:
	void CreateMesh();
	void Binding();

public:
	CLONE(CLandScape);

public:
	CLandScape();
	~CLandScape();
};