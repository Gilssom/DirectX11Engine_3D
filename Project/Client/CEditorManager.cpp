#include "pch.h"
#include "CEditorManager.h"

#include <Engine\\CRenderManager.h>

#include "CGameObjectEx.h"
#include <Engine\\components.h>

#include "CEditorCameraScript.h"

CEditorManager::CEditorManager()
{

}

CEditorManager::~CEditorManager()
{
	Safe_Del_Vector(m_vecEditorObject);
}

void CEditorManager::Init()
{
	CGameObject* pEditorCam = new CGameObjectEx;
	pEditorCam->AddComponent(new CTransform);
	pEditorCam->AddComponent(new CCamera);
	// Editor �� Script �� Script Project �� ���� ������ ����.
	pEditorCam->AddComponent(new CEditorCameraScript);

	pEditorCam->Camera()->LayerCheckAll(0xffffffff);
	pEditorCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pEditorCam->Camera()->SetFar(100000.f);

	CRenderManager::GetInst()->RegisterEditorCamera(pEditorCam->Camera());

	m_vecEditorObject.push_back(pEditorCam);
}

void CEditorManager::Tick()
{
	for (size_t i = 0; i < m_vecEditorObject.size(); i++)
	{
		m_vecEditorObject[i]->Tick();
	}

	for (size_t i = 0; i < m_vecEditorObject.size(); i++)
	{
		m_vecEditorObject[i]->FinalTick();
	}
}