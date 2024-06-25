#pragma once
#include "ComponentUI.h"

class CScript;

/// <summary>
/// GameObject 는 Script 를 한개만 가지고 있다는 조건이 없다. (여러개 소유 가능)
/// vector 로 개별적으로 Script 가 관리되고 있다.
/// 
/// Script UI 는 Script 하나만을 담당하여 Editor 에 나열이 될 것이다.
/// </summary>
class ScriptUI : public ComponentUI
{
private:
	CScript* m_TargetScript;

public:
	void SetScript(CScript* script);

public:
	virtual void Render_Tick() override;

public:
	void Render_ScriptName();

public:
	ScriptUI();
	~ScriptUI();
};