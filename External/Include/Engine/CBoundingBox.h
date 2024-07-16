#pragma once
#include "CComponent.h"

class CBoundingBox : public CComponent
{
private:
	Vec3	m_OffsetPos;
	float	m_Radius;
	bool	m_Absolute;
	Matrix	m_WorldMat;

public:
	void SetOffset(Vec3 offset) { m_OffsetPos = offset; }
	Vec3 GetOffset() { return m_OffsetPos; }
	Vec3 GetWorldPos() { return m_WorldMat.Translation(); }

	void SetRadius(float radius) { m_Radius = radius; }
	float GetRadius() { return m_Radius; }

	void SetAbsolute(bool absolute) { m_Absolute = absolute; }
	bool IsAbsolute() { return m_Absolute; }

public:
	virtual void FinalTick() override;

	virtual void SaveToLevelFile(FILE* file);
	virtual void LoadFromLevelFile(FILE* file);

public:
	CLONE(CBoundingBox);

public:
	CBoundingBox();
	~CBoundingBox();
};