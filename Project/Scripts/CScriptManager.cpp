#include "pch.h"
#include "CScriptManager.h"

#include "CMissileScript.h"

void CScriptManager::GetScriptInfo(vector<wstring>& vec)
{
	vec.push_back(L"CCameraMoveScript");
	vec.push_back(L"CMissileScript");
}

CScript* CScriptManager::GetScript(const wstring& strScriptName)
{
	if (L"CMissileScript" == strScriptName)
		return new CMissileScript;
	return nullptr;
}

CScript* CScriptManager::GetScript(UINT scriptType)
{
	switch (scriptType)
	{
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
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
	}
	return nullptr;
}