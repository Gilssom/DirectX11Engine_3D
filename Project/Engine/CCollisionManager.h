#pragma once
#include "Singletone.h"

union COL_ID
{
	struct
	{
		UINT LeftID;
		UINT RightID;
	};

	LONGLONG ID;
};

class CCollider2D;

class CCollisionManager : public CSingleton<CCollisionManager>
{
	SINGLE(CCollisionManager)

private:
	UINT	 m_Matrix[(UINT)MAX_LAYER];
	map<LONGLONG, bool> m_ColInfo;


public:
	void Tick();


public:
	void LayerCheck(UINT layerLeftIndex, UINT layerRightIndex);


private:
	void CollisionBtwLayer(UINT left, UINT right);
	void CollisionBtwCollider2D(CCollider2D* leftCol, CCollider2D* rightCol);

	bool IsCollision(CCollider2D* leftCol, CCollider2D* rightCol);

public:
	void SaveToLevelFile(FILE* file);
	void LoadToLevelFile(FILE* file);
};

