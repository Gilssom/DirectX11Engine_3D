#pragma once
#include <Engine\CScript.h>

class CNpcScript : public CScript
{
public:
	virtual void Begin() override;
	virtual void Tick() override;

public:
	bool IsClicked();

public:
	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CNpcScript);

public:
	CNpcScript();
	~CNpcScript();
};

