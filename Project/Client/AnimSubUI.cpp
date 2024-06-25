#include "pch.h"
#include "AnimSubUI.h"

AnimSubUI::AnimSubUI(const string& name, const string& id)
	: EditorUI(name, id)
	, m_Owner(nullptr)
{

}

AnimSubUI::~AnimSubUI()
{

}

SpriteView* AnimSubUI::GetSpriteView()
{
	return (SpriteView*)m_Owner->GetSubUI(SPRITEVIEW);
}

AnimPreView* AnimSubUI::GetAnimPreView()
{
	return (AnimPreView*)m_Owner->GetSubUI(PREVIEW);
}

AnimDetail* AnimSubUI::GetAnimDetail()
{
	return (AnimDetail*)m_Owner->GetSubUI(ANIMDETAIL);
}