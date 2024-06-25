#pragma once
#include "Singletone.h"

class CGameObject;

class CDbgRenderManager : public CSingleton<CDbgRenderManager>
{
	SINGLE(CDbgRenderManager);

private:
	// �߰��� ���� ������ ����ϰ� �Ͼ �� �ֱ� ������ ����Ʈ
	list<tDebugShapeInfo>	m_ShapeInfo;
	CGameObject*			m_DebugRenderObj; // Debug�� Object ( Level �� ���� �ȵ� )

public:
	void AddDebugShapeInfo(const tDebugShapeInfo& info) { m_ShapeInfo.push_back(info); }
	void Render();

};

