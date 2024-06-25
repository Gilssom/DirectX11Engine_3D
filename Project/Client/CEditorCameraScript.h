#pragma once
#include <Engine\\CScript.h>

class CEditorCameraScript : public CScript
{
private:
	float m_Speed;

public:
	virtual void Tick() override;

private:
	void MoveByPerspective();
	void MoveByOrthographic();

public:
	CLONE(CEditorCameraScript);

public:
	CEditorCameraScript();
	~CEditorCameraScript();
};

