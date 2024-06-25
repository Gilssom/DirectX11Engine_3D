#pragma once
#include <Engine\CScript.h>

class CObjectScript : public CScript
{
private:
	int m_Health;

public:
	virtual void Begin() override;
	virtual void Tick() override;

	virtual void BeginOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;
	virtual void Overlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;
	virtual void EndOverlap(CCollider2D* ownerCollider, CGameObject* otherObject, CCollider2D* otehrCollider) override;

public:
	CLONE(CObjectScript);


public:
	CObjectScript();
	~CObjectScript();
};