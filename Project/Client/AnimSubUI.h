#pragma once
#include "EditorUI.h"

#include "AnimationEditor.h"

class AnimSubUI : public EditorUI
{
private:
	AnimationEditor*	m_Owner;

public:
	AnimationEditor* GetOwner() { return m_Owner; }

	class SpriteView* GetSpriteView();
	class AnimPreView* GetAnimPreView();
	class AnimDetail* GetAnimDetail();

public:
	virtual void Activate() {}
	virtual void Deactivate() {}

public:
	AnimSubUI(const string& name, const  string& id);
	~AnimSubUI();

	
	friend class AnimationEditor;
};