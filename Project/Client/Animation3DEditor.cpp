#include "pch.h"
#include "Animation3DEditor.h"

#include "CImGuiManager.h"
#include "Anim3DSubUI.h"

#include "Anim3DPreView.h"
#include "Anim3DDetail.h"

Animation3DEditor::Animation3DEditor()
	: EditorUI("3DAnimationEditor", "##3DAnimationEditor")
	, m_Sub{}
	, m_PrevActive{}
{
	//m_Sub[SPRITEVIEW] = CImGuiManager::GetInst()->FindEditorUI<SpriteView>("Sprite View");
	//m_Sub[PREVIEW_3D] = CImGuiManager::GetInst()->FindEditorUI<Anim3DPreView>("Anim 3D PreView");
	//m_Sub[ANIMDETAIL] = CImGuiManager::GetInst()->FindEditorUI<AnimDetail>("Anim Detail");

	m_Sub[PREVIEW_3D] = CImGuiManager::GetInst()->FindEditorUI<Anim3DPreView>("Anim 3D PreView");
	m_Sub[DETAIL_3D] = CImGuiManager::GetInst()->FindEditorUI<Anim3DDetail>("Anim 3D Detail");

	for (int i = 0; i < 3; i++)
	{
		if (m_Sub[i] != nullptr)
		{
			m_Sub[i]->SetActive(true);
			m_Sub[i]->m_Owner = this;
		}
	}
}

Animation3DEditor::~Animation3DEditor()
{

}

void Animation3DEditor::Render_Tick()
{
	//bool IsActive[3] = { m_Sub[0]->IsActive(), m_Sub[1]->IsActive(), m_Sub[2]->IsActive() };

	bool IsActive[3] = { false, m_Sub[1]->IsActive() , m_Sub[2]->IsActive() };

	m_Sub[PREVIEW_3D]->SetActive(IsActive[PREVIEW_3D]);
	m_Sub[DETAIL_3D]->SetActive(IsActive[DETAIL_3D]);
}

void Animation3DEditor::Activate()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_PrevActive[i])
		{
			if (m_Sub[i] != nullptr)
				m_Sub[i]->SetActive(true);
		}
	}
}

void Animation3DEditor::Deactivate()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_Sub[i] != nullptr)
		{
			m_PrevActive[i] = m_Sub[i]->IsActive();
			m_Sub[i]->SetActive(false);
		}
	}
}