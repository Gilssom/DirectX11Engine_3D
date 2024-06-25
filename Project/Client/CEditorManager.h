#pragma once
#include <Engine\\Singletone.h>

class CEditorManager : public CSingleton<CEditorManager>
{
	SINGLE(CEditorManager);

private:
	vector<CGameObject*> m_vecEditorObject;

public:
	void Init();
	void Tick();

};