#include "pch.h"
#include "CScriptManager.h"

#include "CCameraMoveScript.h"
#include "CMissileScript.h"
#include "CPlayerScript.h"
#include "CAnimStateMachine.h"

void CScriptManager::GetScriptInfo(vector<wstring>& vec)
{
	vec.push_back(L"CCameraMoveScript");
	vec.push_back(L"CMissileScript");
	vec.push_back(L"CPlayerScript");
	vec.push_back(L"CAnimStateMachine");
}

CScript* CScriptManager::GetScript(const wstring& strScriptName)
{
	if (L"CCameraMoveScript" == strScriptName)
		return new CCameraMoveScript;
	if (L"CMissileScript" == strScriptName)
		return new CMissileScript;
	if (L"CPlayerScript" == strScriptName)
		return new CPlayerScript;
	if (L"CAnimStateMachine" == strScriptName)
		return new CAnimStateMachine;
	return nullptr;
}

CScript* CScriptManager::GetScript(UINT scriptType)
{
	switch (scriptType)
	{
	case (UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return new CCameraMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::ANIMSTATEMACHINE:
		return new CAnimStateMachine;
		break;
	}
	return nullptr;
}

const wchar_t* CScriptManager::GetScriptName(CScript* pScript)
{
	switch ((SCRIPT_TYPE)pScript->GetScriptType())
	{
	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return L"CCameraMoveScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";

	case SCRIPT_TYPE::ANIMSTATEMACHINE:
		return L"CAnimStateMachine";
		break;
	}
	return nullptr;
}