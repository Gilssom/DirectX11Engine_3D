#pragma once
#include "EditorUI.h"

#include "Animation3DEditor.h"

class Anim3DSubUI : public EditorUI
{
private:
	Animation3DEditor* m_Owner;

public:
	Animation3DEditor* GetOwner() { return m_Owner; }

	class Anim3DPreView*	GetAnimPreView();
	class Anim3DDetail*		GetAnimDetail();

public:
	virtual void Activate() {}
	virtual void Deactivate() {}

public:
	Anim3DSubUI(const string& name, const  string& id);
	~Anim3DSubUI();


	friend class Animation3DEditor;
};