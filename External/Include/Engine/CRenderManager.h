#pragma once

#include "CTexture.h"

enum class RENDER_MODE
{
	PLAY,
	EDITOR,
};

class CCamera;
class CLight2D;
class CStructuredBuffer;

class CRenderManager : public CSingleton<CRenderManager>
{
	SINGLE(CRenderManager);

private:
	vector<CCamera*>	m_vecCam;
	CCamera*			m_EditorCam;

	// 멤버 함수 포인터
	void(CRenderManager::*Render_Func)(void);

	vector<CLight2D*>	m_vecLight2D;		// 매 프레임마다 등록해야함 ( = Layer )
	CStructuredBuffer*	m_Light2DBuffer;	// 구조화 버퍼

	Ptr<CTexture>		m_RenderTargetCopyTex;	// Post Process 를 위한 Copy Render Target Texture


public:
	void RegisterCamera(CCamera* camera, int priority);
	void ClearRegisterCamera() { m_vecCam.clear(); }
	void RegisterEditorCamera(CCamera* editorCam) { m_EditorCam = editorCam; }

	vector<CCamera*>& GetRegisteredCamera() { return m_vecCam; }

	int RegisterLight2D(CLight2D* light2D) 
	{ 
		m_vecLight2D.push_back(light2D);
		return (int)m_vecLight2D.size() - 1; // 마지막 Index 알려줌
	}

	void CopyRenderTarget();

public:
	void Init();
	void Tick();
	void Render();

	void ChangeRenderMode(RENDER_MODE renderMode)
	{
		renderMode == RENDER_MODE::PLAY ? Render_Func = &CRenderManager::Render_Play : Render_Func = &CRenderManager::Render_Editor;
	}

private:
	void Render_Play();
	void Render_Editor();

private:
	void DataBinding();
	void DataClear();
};

