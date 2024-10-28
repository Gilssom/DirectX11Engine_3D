#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	ANIMSTATEMACHINE,
	BACKGROUNDSCRIPT,
	BOSSSCRIPT,
	CAMERAMOVESCRIPT,
	DAMAGESCRIPT,
	GATESCRIPT,
	GOBLINSCRIPT,
	LIGHTONSCRIPT,
	MISSILESCRIPT,
	MONSTERSCRIPT,
	NPCSCRIPT,
	OBJECTSCRIPT,
	ONTENTS_ENUM,
	ONTENTS_STRUCT,
	PLAYERSCRIPT,
	PLAYERSKILLSCRIPT,
	SKILLMANAGER,
	TAUSCRIPT,
	TESTSCRIPT,
	UIMANAGER,
	WATERSCRIPT,
	IBUTTON,
	IDAMAGE,
	IELEMENT,
	IHPBAR,
	ILOBBY,
	IMINIMAP,
	IMONSTERHP,
	IMPBAR,
	IRESULT,
	ISKILL,
	ISTATUS,
};

using namespace std;

class CScript;

class CScriptManager
{
public:
	static void GetScriptInfo(vector<wstring>& vec);
	static CScript* GetScript(const wstring& strScriptName);
	static CScript* GetScript(UINT scriptType);
	static const wchar_t* GetScriptName(CScript* pScript);
};
