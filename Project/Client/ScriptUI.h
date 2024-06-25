#pragma once
#include "ComponentUI.h"

class CScript;

/// <summary>
/// GameObject �� Script �� �Ѱ��� ������ �ִٴ� ������ ����. (������ ���� ����)
/// vector �� ���������� Script �� �����ǰ� �ִ�.
/// 
/// Script UI �� Script �ϳ����� ����Ͽ� Editor �� ������ �� ���̴�.
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