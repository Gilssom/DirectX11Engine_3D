#pragma once

#include <Engine\\CGameObject.h>

// Editor �� GameObject
class CGameObjectEx : public CGameObject
{
public:
	virtual void FinalTick() override;
};

