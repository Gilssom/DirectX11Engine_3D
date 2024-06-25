#pragma once
#include "CLevelManager.h"
#include "CSoundManager.h"

#include <Scripts\CPlayerScript.h>

class CLevelTrigger
{
private:

public:
	void onPlayerOverlap(CPlayerScript* player);

public:
	CLevelTrigger();
	~CLevelTrigger();
};