#pragma once
#include "EditorUI.h"

#include <Engine\\CGameObject.h>

// 추상 클래스 (상속 용도)
class ComponentUI : public EditorUI
{
private:
	CGameObject*			m_TargetObject;
	const COMPONENT_TYPE	m_Type;


public:
	void SetTarget(CGameObject* target);
	CGameObject* GetTarget() { return m_TargetObject; }

	COMPONENT_TYPE GetComponentType() { return m_Type; }

public:
	virtual void Render_Tick() = 0;

protected:
	void Render_Title();

public:
	ComponentUI(const string& name, const  string& id, COMPONENT_TYPE type);
	~ComponentUI();
};

