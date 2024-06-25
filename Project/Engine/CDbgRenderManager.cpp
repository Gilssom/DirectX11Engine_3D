#include "pch.h"
#include "CDbgRenderManager.h"

#include "CTimeManager.h"
#include "CAssetManager.h"
#include "CMesh.h"
#include "CMaterial.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

// 싱글톤 객체라서 활용할 때마다 하나의 오브젝트로 필요한 정보들만 모아서 렌더링을 하면 된다. (모듈화)
CDbgRenderManager::CDbgRenderManager()
	: m_DebugRenderObj(nullptr)
{
	m_DebugRenderObj = new CGameObject;
	// Debug용 위치
	m_DebugRenderObj->AddComponent(new CTransform);  

	// Debug용 렌더
	m_DebugRenderObj->AddComponent(new CMeshRender); 

	// Debug용 재질
	m_DebugRenderObj->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DebugShapeMaterial"));
}

CDbgRenderManager::~CDbgRenderManager()
{
	delete m_DebugRenderObj;
	m_DebugRenderObj = nullptr;
}

void CDbgRenderManager::Render()
{
	list<tDebugShapeInfo>::iterator iter = m_ShapeInfo.begin();

	for (; iter != m_ShapeInfo.end();)
	{
		// World 행렬 연산이 안되어 있으면 진행
		if (iter->matWorld == XMMatrixIdentity())
		{
			m_DebugRenderObj->Transform()->SetRelativePos(iter->Position);
			m_DebugRenderObj->Transform()->SetRelativeScale(iter->Scale);
			m_DebugRenderObj->Transform()->SetRelativeRotation(iter->Rotation);
			m_DebugRenderObj->Transform()->FinalTick();
		}
		else
		{
			m_DebugRenderObj->Transform()->SetWorldMat(iter->matWorld);
		}

		// Mesh 참조
		switch (iter->Shape)
		{
		case DEBUG_SHAPE::RECT:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh_Debug"));
			break;
		case DEBUG_SHAPE::CIRCLE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"CircleMesh_Debug"));
			break;
		case DEBUG_SHAPE::LINE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"LineMesh"));
			break;
		case DEBUG_SHAPE::CUBE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"CubeMesh"));
			break;
		case DEBUG_SHAPE::SPHERE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
			break;
		default:
			break;
		}
		
		// Debug용 Object Render
		m_DebugRenderObj->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, iter->Color);
		m_DebugRenderObj->Render();

		iter->Age += DT;

		if (iter->Duration <= iter->Age)
		{
			iter = m_ShapeInfo.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}