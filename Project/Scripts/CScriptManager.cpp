#include "pch.h"
#include "CScriptManager.h"

#include "CCameraMoveScript.h"
#include "CMissileScript.h"
#include "CPlayerScript.h"
#include "CAnimStateMachine.h"
#include "CWaterScript.h"

void CScriptManager::GetScriptInfo(vector<wstring>& vec)
{
	vec.push_back(L"CCameraMoveScript");
	vec.push_back(L"CMissileScript");
	vec.push_back(L"CPlayerScript");
	vec.push_back(L"CAnimStateMachine");
	vec.push_back(L"CWaterScript");
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
	if (L"CWaterScript" == strScriptName)
		return new CWaterScript;
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
	case (UINT)SCRIPT_TYPE::WATERSCRIPT:
		return new CWaterScript;
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

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";

	case SCRIPT_TYPE::ANIMSTATEMACHINE:
		return L"CAnimStateMachine";

	case SCRIPT_TYPE::WATERSCRIPT:
		return L"CWaterScript";
	}
	return nullptr;
}