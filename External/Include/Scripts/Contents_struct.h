#pragma once

#include <Engine\define.h>
#include <Engine\global.h>

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