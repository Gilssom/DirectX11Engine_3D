#pragma once

#include "CTexture.h"

enum class RENDER_MODE
{
	PLAY,
	EDITOR,
};

class CCamera;
class CLight2D;
class CLight3D;
class CStructuredBuffer;

class CRenderManager : public CSingleton<CRenderManager>
{
	SINGLE(CRenderManager);

private:
	vector<CCamera*>	m_vecCam;
	CCamera*			m_EditorCam;

	// ��� �Լ� ������
	void(CRenderManager::*Render_Func)(void);

	vector<CLight2D*>	m_vecLight2D;				// �� �����Ӹ��� ����ؾ��� ( = Layer )
	CStructuredBuffer*	m_Light2DBuffer;			// ����ȭ ����

	vector<CLight3D*>	m_vecLight3D;				// �� �����Ӹ��� ����ؾ��� ( = Layer )
	CStructuredBuffer*	m_Light3DBuffer;			// ����ȭ ����

	Ptr<CTexture>		m_RenderTargetCopyTex;		// Post Process �� ���� Copy Render Target Texture

	class CMRT*			m_MRT[(UINT)MRT_TYPE::END]; // MRT ���� �� ����

	bool				m_OutputTarget;
	Ptr<CTexture>		m_OutputTargetTexture;

public:
	void RegisterCamera(CCamera* camera, int priority);
	void ClearRegisterCamera() { m_vecCam.clear(); }
	void RegisterEditorCamera(CCamera* editorCam) { m_EditorCam = editorCam; }

	void SetOutputTarget(bool set, Ptr<CTexture> targetTex) 
	{ 
		m_OutputTarget = set; 
		m_OutputTargetTexture = targetTex; 
	}

	bool IsOutputTarget() { return m_OutputTarget; }
	Ptr<CTexture> GetOutputTarget() { return m_OutputTargetTexture; }

	vector<CCamera*>& GetRegisteredCamera() { return m_vecCam; }

	int RegisterLight2D(CLight2D* light2D) 
	{ 
		m_vecLight2D.push_back(light2D);
		return (int)m_vecLight2D.size() - 1; // ������ Index �˷���
	}

	int RegisterLight3D(CLight3D* light3D)
	{
		m_vecLight3D.push_back(light3D);
		return (int)m_vecLight3D.size() - 1; // ������ Index �˷���
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
	void CreateMRT();
	void ClearMRT();
};