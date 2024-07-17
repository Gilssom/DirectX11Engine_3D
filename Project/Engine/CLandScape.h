#pragma once
#include "CRenderComponent.h"

class CLandScape : public CRenderComponent
{
private:
	UINT	m_FaceX;
	UINT	m_FaceZ;

public:
	void SetFace(UINT x, UINT z);

public:
	void Init();
	virtual void FinalTick() override;
	virtual void Render() override;

private:
	void CreateMesh();

public:
	CLONE(CLandScape);

public:
	CLandScape();
	~CLandScape();
};