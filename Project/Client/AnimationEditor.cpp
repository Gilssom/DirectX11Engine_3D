#include "pch.h"
#include "AnimationEditor.h"

#include "CImGuiManager.h"
#include "AnimSubUI.h"

#include "SpriteView.h"
#include "AnimPreView.h"
#include "AnimDetail.h"

#include <Engine\CAnim2D.h>

AnimationEditor::AnimationEditor()
	: EditorUI("AnimationEditor", "##AnimationEditor")
	, m_Sub{}
	, m_PrevActive{}
{
	m_Sub[SPRITEVIEW] = CImGuiManager::GetInst()->FindEditorUI<SpriteView>("Sprite View");
	m_Sub[PREVIEW] = CImGuiManager::GetInst()->FindEditorUI<AnimPreView>("Anim PreView");
	m_Sub[ANIMDETAIL] = CImGuiManager::GetInst()->FindEditorUI<AnimDetail>("Anim Detail");

	for (int i = 0; i < 3; i++)
	{
		m_Sub[i]->SetActive(true);
		m_Sub[i]->m_Owner = this;
	}
}

AnimationEditor::~AnimationEditor()
{

}

void AnimationEditor::Render_Tick()
{
	bool IsActive[3] = { m_Sub[0]->IsActive(), m_Sub[1]->IsActive(), m_Sub[2]->IsActive() };
    
	m_Sub[SPRITEVIEW]->SetActive(IsActive[SPRITEVIEW]);
	m_Sub[PREVIEW]->SetActive(IsActive[PREVIEW]);
	m_Sub[ANIMDETAIL]->SetActive(IsActive[ANIMDETAIL]);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File##AnimationEditor"))
        {
            if (ImGui::MenuItem("Load Sprite##AnimaiontEditor"))
            {
                wchar_t Buff[255] = {};

                OPENFILENAME ofn = {};

                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = nullptr;
                ofn.lpstrFile = Buff;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = 255;
                ofn.lpstrFilter = L"sprite\0*.png\0All\0*.*";
                ofn.nFilterIndex = 0;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;

                // Ž��â �ʱ� ��ġ ����
                wstring strInitPath = CPathManager::GetInst()->GetContentPath() + wstring(L"texture\\");
                ofn.lpstrInitialDir = strInitPath.c_str();
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn))
                {
                    wstring RelativePath = CPathManager::GetInst()->GetRelativePath(Buff);
                    // Sprite Texture �ҷ�����
                    Ptr<CTexture> pLoadedTexture = CAssetManager::GetInst()->Load<CTexture>(RelativePath, RelativePath);

                    SpriteView* pSpriteView = (SpriteView*)GetSubUI(SPRITEVIEW);
                    pSpriteView->SetSpriteTexture(pLoadedTexture);
                }
            }

            SpriteView* pSpriteView = (SpriteView*)m_Sub[SPRITEVIEW];

            // Sprite Texture ��������
            Ptr<CTexture> pSprite = pSpriteView->GetSpriteTexture();

            ImGui::BeginDisabled(pSprite == nullptr);
            if (ImGui::MenuItem("Save File##AnimaiontEditor"))
            {
                if (pSprite == nullptr)
                    return;

                wchar_t Buff[255] = {};

                OPENFILENAME ofn = {};
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = nullptr;
                ofn.lpstrFile = Buff;        // ��� ���
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = 255;
                ofn.lpstrFilter = L"Animation\0*.anim\0All\0*.*";
                ofn.nFilterIndex = 0;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;

                // Ž��â �ʱ� ��ġ ����
                wstring strInitPath = CPathManager::GetInst()->GetContentPath() + wstring(L"Animation\\");
                ofn.lpstrInitialDir = strInitPath.c_str();
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetSaveFileName(&ofn))
                {
                    // ������ ���� ��ü ��������
                    vector<Frame>& pFrames = pSpriteView->GetFrames();

                    vector<tAnim2DFrame> vframes;

                    AnimPreView* pPreView = (AnimPreView*)m_Sub[PREVIEW];
                    float speed = pPreView->GetSpeed();

                    for (int i = 0; i < pFrames.size(); i++)
                    {
                        tAnim2DFrame frame = {};

                        frame.LeftTop.x = pFrames[i].topLeft.x;
                        frame.LeftTop.y = pFrames[i].topLeft.y;

                        frame.Duration = (float)speed;

                        frame.SliceSize.x = pFrames[i].bottomRight.x - pFrames[i].topLeft.x;
                        frame.SliceSize.y = pFrames[i].bottomRight.y - pFrames[i].topLeft.y;

                        vframes.push_back(frame);
                    }

                    wstring RelativeName = CPathManager::GetInst()->GetRelativeName(Buff, wstring(L"Animation\\"));

                    // CAnim Create �Լ� �����ε��ؼ� ���� ����
                    CAnim2D* pAnim = new CAnim2D;
                    pAnim->SetName(RelativeName);
                    pAnim->Create(pSprite, vframes, pFrames.size());
                    pAnim->Save(L"Animation\\");
                }          
            }
            ImGui::EndDisabled();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void AnimationEditor::Activate()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_PrevActive[i])
			m_Sub[i]->SetActive(true);
	}
}

void AnimationEditor::Deactivate()
{
	for (int i = 0; i < 3; i++)
	{
		m_PrevActive[i] = m_Sub[i]->IsActive();
		m_Sub[i]->SetActive(false);
	}
}