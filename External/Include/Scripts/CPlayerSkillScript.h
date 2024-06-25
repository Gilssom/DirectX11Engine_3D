#pragma once
#include <Engine\\CScript.h>

#include "CSkillManager.h"

class CPlayerSkillScript : public CScript
{
private:
	eSkillType		m_SkillType;
	sSkill_Info		m_SkillInfo;

	bool			m_IsMove;
	float			m_Speed;
	float			m_LifeTimer;

public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	void SetSkillType(eSkillType skillType) { m_SkillType = skillType; }
	void SetSkillInfo(sSkill_Info skillInfo) { m_SkillInfo = skillInfo; }

	eSkillType GetSkillType() { return m_SkillType; }
	sSkill_Info GetSkillInfo() { return m_SkillInfo; }

	void FinalExecuteSkill();

public:
	void ExecuteSaintWall();
	void SaintWallHit();

	void ExecuteRefentence();
	//void RefentenceHit();

	void ExecuteGrandCrossCrash();
	void GrandCrossCrashAura();

	void ExecuteHolyFlash();
	void HolyFlashBall();

	void ExecuteCrusaderBuff();

	void PlaySoundEffect(const wstring& soundPath);

public:
	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CPlayerSkillScript);

public:
	CPlayerSkillScript();
	~CPlayerSkillScript();

	friend class CPlayerScript;
};

