#include "pch.h"
#include "ScriptUI.h"

#include <Scripts\\CScriptManager.h>
#include <Engine\\CScript.h>
#include "ParamUI.h"

ScriptUI::ScriptUI()
	: ComponentUI("Script", "##ScriptUI", COMPONENT_TYPE::SCRIPT)
{
	SIZE;
}

ScriptUI::~ScriptUI()
{

}

void ScriptUI::SetScript(CScript* script)
{
	m_TargetScript = script;

	string strScriptName = ToString(CScriptManager::GetScriptName(m_TargetScript));

	SetName(strScriptName.c_str());
}

void ScriptUI::Render_Tick()
{
	Render_ScriptName();

	// ����� Script �� Property ������ Ȯ��
	const vector<tScriptProperty>& vecProperty = m_TargetScript->GetScriptProperty();

	ParamUI::SetDescWidth(140);

	for (size_t i = 0; i < vecProperty.size(); i++)
	{
		switch (vecProperty[i].Type)
		{
		case PROPERTY_TYPE::INT:
			ParamUI::Param_DragInt(vecProperty[i].Desc, (int*)vecProperty[i].pData, 1);
			break;
		case PROPERTY_TYPE::FLOAT:
			ParamUI::Param_DragFloat(vecProperty[i].Desc, (float*)vecProperty[i].pData, 1);
			break;
		case PROPERTY_TYPE::VEC2:
			break;
		case PROPERTY_TYPE::VEC3:
			break;
		case PROPERTY_TYPE::VEC4:
			ParamUI::Param_DragVec4(vecProperty[i].Desc, (Vec4*)vecProperty[i].pData, 1);
			break;
		case PROPERTY_TYPE::TEXTURE:
			break;
		case PROPERTY_TYPE::GAMEOBJECT:
			ParamUI::Param_GameObject(vecProperty[i].Desc, (CGameObject*&)vecProperty[i].pData);
			break;
		}
	}
}

void ScriptUI::Render_ScriptName()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2.f / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2.f / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2.f / 7.0f, 0.8f, 0.8f));

	wstring strScriptName = CScriptManager::GetScriptName(m_TargetScript);
	ImGui::Button(ToString(strScriptName).c_str());

	ImGui::PopStyleColor(3);
	ImGui::PopID();
}

// �����ϱ� ���� �ʿ��� ��ɵ�

// 1. Script �����͸� �˾Ƶ�, ���� Script ���� Class Name �� �˾Ƴ� �� �ִ�.

// 2. ����(����)�� ��� Script List �� �˾Ƴ� �� �־�� �Ѵ�.

// 3. Script �� Class Name �����ε� Script ��ü�� ������ų �� �־�� �Ѵ�.

// ��, Script �� �ڵ�ȭ ����� ������������ �ʿ���.