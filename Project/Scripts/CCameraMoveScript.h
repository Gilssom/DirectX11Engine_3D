#pragma once
#include <Engine\\CScript.h>

class CCameraMoveScript : public CScript
{
private:
	CGameObject*	m_TargetObject;
	float			m_Speed;

public:
	virtual void Tick() override;

private:
	void MoveByPerspective();
	void MoveByOrthographic();

public:
	void SetTargetObject(CGameObject* targetObject) { m_TargetObject = targetObject; }

public:
	CLONE(CCameraMoveScript);

public:
	CCameraMoveScript();
	~CCameraMoveScript();
};

