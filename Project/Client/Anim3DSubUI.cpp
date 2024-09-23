#include "pch.h"
#include "Anim3DSubUI.h"

Anim3DSubUI::Anim3DSubUI(const string& name, const string& id)
	: EditorUI(name, id)
	, m_Owner(nullptr)
{

}

Anim3DSubUI::~Anim3DSubUI()
{

}

Anim3DPreView* Anim3DSubUI::GetAnimPreView()
{
	return(Anim3DPreView*)m_Owner->GetSubUI(PREVIEW_3D);
}

Anim3DDetail* Anim3DSubUI::GetAnimDetail()
{
	return(Anim3DDetail*)m_Owner->GetSubUI(DETAIL_3D);
}
