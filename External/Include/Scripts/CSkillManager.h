#pragma once

enum class eSkillType
{
	// Normal Attack
	Attack_0,
	Attack_1,
	Attack_2,

	// Attack Skill
	SaintWall,
	GrandCrossCrash,
	GrandCrossCrash_1,
	GrandCrossCrash_Aura,
	RefentenceSmash,
	HolyFlash,
	HolyFlash_Ball,

	// Hit Effect
	SaintWallHit,

	// Buff Skill
	CrusaderBuff,

	End,
};

struct sSkill_Info
{
	// Create Pos
	Vec3 pos;

	// Direction
	bool isLeft; // true = Left,  false = Right

	// Animation
	wstring animPath;
	wstring animName;

	float coolDownTime;
	float lastUsedTime = std::numeric_limits<float>::lowest();

	int useMana;

	bool isAbsolute = false;
};

class CPlayerScript;

class CSkillManager
{
public:
	static CSkillManager& Instance()
	{
		static CSkillManager instance;
		return instance;
	}

private:
	std::map<eSkillType, sSkill_Info>	m_SkillInfoMap;
	CPlayerScript*						m_UsedPlayer;
	Ptr<CPrefab>						m_SkillPrefab;
	float								m_CurTime;

public:
	void Init();
	void Tick();
	void RegisterSkill(eSkillType skillType, const sSkill_Info& info) { m_SkillInfoMap[skillType] = info; };
	CGameObject* CreateSkill(eSkillType skillType, Vec3 spawnPos, bool isLeft);
	void UseSkill(eSkillType skillType, Vec3 spawnPos, bool isLeft);
	bool SkillCoolTime(eSkillType skillType);
	bool SkillUseMana(eSkillType skillType, int& curMp);
	float GetSkillCoolTime (eSkillType skillType) const;

public:
	void SetUsedPlayer(CPlayerScript* player) { m_UsedPlayer = player; }
	CPlayerScript* GetUsedPlayer() { return m_UsedPlayer; }

private:
	CGameObject* Instantiate(Ptr<CPrefab> prefab, int layerIdx, const Vec3& worldPos);

public:
	CSkillManager();
	~CSkillManager();
};