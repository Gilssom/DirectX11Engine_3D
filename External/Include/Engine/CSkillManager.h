#pragma once
#include "Singletone.h"

enum class eSkillType
{
	// Normal Attack
	Attack_0,
	Attack_1,
	Attack_2,

	// Attack Skill
	SaintWall,
	GrandCrossCrash,
	RefantansSmash,
	MiracleShine,

	// Buff Skill
	CrusaderBuff,

	End,
};

struct sSkill_Info
{
	// Create Pos
	Vec3 pos;

	// Direction
	//bool isLeft; // true = Left,  false = Right

	// Animation
	wstring animPath;
	wstring animName;
};

class CSkillManager : public CSingleton<CSkillManager>
{
	SINGLE(CSkillManager)

private:
	map<eSkillType, sSkill_Info> m_SkillInfoMap;

	Ptr<CPrefab>    m_SkillPrefab;

public:
	void Init();
	void RegisterSkill(eSkillType skillType, const sSkill_Info& info) { m_SkillInfoMap[skillType] = info; }
	std::unique_ptr<CGameObject> CreateSkill(eSkillType skillType, Vec3 spawnPos);
	void UseSkill();

public:
	CGameObject* Instantiate(Ptr<CPrefab> prefab, int layerIdx, const Vec3& worldPos);

public:
	void SaveToLevelFile(FILE* file);
	void LoadToLevelFile(FILE* file);
};