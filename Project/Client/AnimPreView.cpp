#include "pch.h"
#include "AnimPreView.h"

#include "AnimDetail.h"

#include <Engine\\CTimeManager.h>

AnimPreView::AnimPreView()
	: AnimSubUI("Anim PreView", "##Anim PreView")
	, m_SpriteView(nullptr)
	, m_PlaySpeed(1.f)
	, m_IsPlaying(false)
	, m_CurFrameIndex(0)
	, m_Timer(0.f)
{

}

AnimPreView::~AnimPreView()
{

}

void AnimPreView::Play()
{
	m_IsPlaying = true;
	m_CurFrameIndex = 0;
	m_Timer = 0.f;
}

void AnimPreView::Stop()
{
	m_IsPlaying = false;
	m_CurFrameIndex = 0;
	m_Timer = 0.f;
}

void AnimPreView::UpdateAnimation()
{
	// Sprite View 가 있고, Animation Play 중 이라면
	if (m_SpriteView != nullptr && m_IsPlaying)
	{
		const vector<Frame>& frames = m_SpriteView->GetFrames();

		if (frames.empty())
			return;
		
		m_Timer += DT_Engine * m_PlaySpeed;

		if (m_Timer >= 1.f)
		{
			m_Timer = 0.f;
			m_CurFrameIndex = (m_CurFrameIndex + 1) % frames.size();
		}
	}
}

void AnimPreView::RenderPreView()
{
	if (m_SpriteView == nullptr)
		return;

	vector<Frame>& frames = m_SpriteView->GetFrames();

	if (frames.empty())
		return;

	Frame& curFrame = frames[m_CurFrameIndex];
	GetAnimDetail()->SetSelectFrame(&curFrame);

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	Ptr<CTexture> pTex = m_SpriteView->GetSpriteTexture();
	ImTextureID TexID = pTex->GetSRV().Get();
	
	ImVec2 imageSize = ImVec2(pTex->GetWidth(), pTex->GetHeight());
	ImVec2 uv0 = ImVec2(curFrame.topLeft.x / imageSize.x, curFrame.topLeft.y / imageSize.y);
	ImVec2 uv1 = ImVec2(curFrame.bottomRight.x / imageSize.x, curFrame.bottomRight.y / imageSize.y);

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 size = ImVec2(curFrame.bottomRight.x - curFrame.topLeft.x, curFrame.bottomRight.y - curFrame.topLeft.y);

	ImGui::Image(TexID, size, uv0, uv1);

	drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(128, 128, 128, 255), 0.0f, 0, 2.0f);
}

void AnimPreView::SelectedFrame()
{
	ImGui::SeparatorText("Frame Selected");

	if (m_SpriteView == nullptr)
		return;

	int curFrameIndex = m_CurFrameIndex;

	if (ImGui::InputInt("##CameraPriority", &curFrameIndex))
	{
		const vector<Frame>& frames = m_SpriteView->GetFrames();

		if (frames.empty())
			return;

		int minValue = 0;
		int maxValue = frames.size() - 1;

		if (curFrameIndex < minValue)
			curFrameIndex = 0;

		if (curFrameIndex > maxValue)
			curFrameIndex = maxValue;

		m_CurFrameIndex = curFrameIndex;
	}
}

void AnimPreView::Render_Tick()
{
	ImGui::SeparatorText("Animation Preview");

	if (ImGui::Button("Play"))
	{
		Play();
	}

	SAME;

	if (ImGui::Button("Stop"))
	{
		Stop();
	}

	ImGui::SliderFloat("Speed", &m_PlaySpeed, 0.1f, 50.0f, "Speed = %.1f");

	UpdateAnimation();

	RenderPreView();

	SelectedFrame();
}

void AnimPreView::Activate()
{

}

void AnimPreView::Deactivate()
{
	m_SpriteView = nullptr;
	m_PlaySpeed = 1.f;
	m_IsPlaying = false;
	m_CurFrameIndex = 0;
	m_Timer = 0.f;
}	