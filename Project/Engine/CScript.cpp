#include "pch.h"
#include "CScript.h"

#include "CTransform.h"


CScript::CScript(UINT scriptType)
	: CComponent(COMPONENT_TYPE::SCRIPT)
	, m_ScriptType(scriptType)
{
}

CScript::~CScript()
{
}

CGameObject* CScript::Instantiate(Ptr<CPrefab> prefab, int layerIdx, const Vec3& worldPos)
{
	CGameObject* pInst = prefab->Instantiate();
	pInst->Transform()->SetRelativePos(worldPos);
	SpawnObject(layerIdx, pInst);

	return pInst;
}