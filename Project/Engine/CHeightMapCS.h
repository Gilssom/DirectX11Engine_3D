#pragma once
#include "CComputeShader.h"

class CHeightMapCS : public CComputeShader
{
private:
	Ptr<CTexture>	m_HeightMapTex;

	Vec2			m_BrushPos;
	Vec2			m_BrushScale;

public:
	void SetBrushPos(Vec2 brushPos) { m_BrushPos = brushPos; }
	void SetBrushScale(Vec2 scale) { m_BrushScale = scale; }
	void SetHeightMap(Ptr<CTexture> heightMap) { m_HeightMapTex = heightMap; }

public:
	virtual int Binding() override;
	virtual void CalculateGroupNum() override;
	virtual void Clear() override;

public:
	CHeightMapCS();
	~CHeightMapCS();
};