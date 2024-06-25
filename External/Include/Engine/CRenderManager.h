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

	// ��� �Լ� ������
	void(CRenderManager::*Render_Func)(void);

	vector<CLight2D*>	m_vecLight2D;		// �� �����Ӹ��� ����ؾ��� ( = Layer )
	CStructuredBuffer*	m_Light2DBuffer;	// ����ȭ ����

	Ptr<CTexture>		m_RenderTargetCopyTex;	// Post Process �� ���� Copy Render Target Texture


public:
	void RegisterCamera(CCamera* camera, int priority);
	void ClearRegisterCamera() { m_vecCam.clear(); }
	void RegisterEditorCamera(CCamera* editorCam) { m_EditorCam = editorCam; }

	vector<CCamera*>& GetRegisteredCamera() { return m_vecCam; }

	int RegisterLight2D(CLight2D* light2D) 
	{ 
		m_vecLight2D.push_back(light2D);
		return (int)m_vecLight2D.size() - 1; // ������ Index �˷���
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

