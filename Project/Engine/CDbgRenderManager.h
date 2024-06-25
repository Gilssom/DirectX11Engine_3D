#pragma once
#include "Singletone.h"

class CGameObject;

class CDbgRenderManager : public CSingleton<CDbgRenderManager>
{
	SINGLE(CDbgRenderManager);

private:
	// 중간에 삭제 생성이 빈번하게 일어날 수 있기 때문에 리스트
	list<tDebugShapeInfo>	m_ShapeInfo;
	CGameObject*			m_DebugRenderObj; // Debug용 Object ( Level 에 들어가면 안됨 )

public:
	void AddDebugShapeInfo(const tDebugShapeInfo& info) { m_ShapeInfo.push_back(info); }
	void Render();

};

