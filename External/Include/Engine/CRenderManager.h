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

	// 멤버 함수 포인터
	void(CRenderManager::*Render_Func)(void);

	vector<CLight2D*>	m_vecLight2D;				// 매 프레임마다 등록해야함 ( = Layer )
	CStructuredBuffer*	m_Light2DBuffer;			// 구조화 버퍼

	vector<CLight3D*>	m_vecLight3D;				// 매 프레임마다 등록해야함 ( = Layer )
	CStructuredBuffer*	m_Light3DBuffer;			// 구조화 버퍼

	Ptr<CTexture>		m_RenderTargetCopyTex;		// Post Process 를 위한 Copy Render Target Texture

	class CMRT*			m_MRT[(UINT)MRT_TYPE::END]; // MRT 관리 및 제어

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
		return (int)m_vecLight2D.size() - 1; // 마지막 Index 알려줌
	}

	int RegisterLight3D(CLight3D* light3D)
	{
		m_vecLight3D.push_back(light3D);
		return (int)m_vecLight3D.size() - 1; // 마지막 Index 알려줌
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