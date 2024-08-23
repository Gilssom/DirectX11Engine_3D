#include "pch.h"
#include "CDbgRenderManager.h"

#include "CTimeManager.h"
#include "CAssetManager.h"
#include "CMesh.h"
#include "CMaterial.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

// �̱��� ��ü�� Ȱ���� ������ �ϳ��� ������Ʈ�� �ʿ��� �����鸸 ��Ƽ� �������� �ϸ� �ȴ�. (���ȭ)
CDbgRenderManager::CDbgRenderManager()
	: m_DebugRenderObj(nullptr)
{
	m_DebugRenderObj = new CGameObject;
	// Debug�� ��ġ
	m_DebugRenderObj->AddComponent(new CTransform);  

	// Debug�� ����
	m_DebugRenderObj->AddComponent(new CMeshRender); 
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
		// World ��� ������ �ȵǾ� ������ ����
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

		// Mesh ����
		switch (iter->Shape)
		{
		case DEBUG_SHAPE::RECT:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh_Debug"));
			m_DebugRenderObj->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DebugShapeMaterial"), 0);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_NONE);
			break;
		case DEBUG_SHAPE::CIRCLE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"CircleMesh_Debug"));
			m_DebugRenderObj->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DebugShapeMaterial"), 0);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_NONE);
			break;
		case DEBUG_SHAPE::LINE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"LineMesh"));
			m_DebugRenderObj->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DebugShapeMaterial"), 0);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_BACK);
			break;
		case DEBUG_SHAPE::CUBE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"CubeMesh_Debug"));
			m_DebugRenderObj->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DebugShapeMaterial"), 0);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_NONE);
			break;
		case DEBUG_SHAPE::SPHERE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
			m_DebugRenderObj->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DebugShapeMaterial"), 0);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_FRONT);
			break;
		case DEBUG_SHAPE::CONE:
			m_DebugRenderObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"ConeMesh"));
			m_DebugRenderObj->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DebugShapeMaterial"), 0);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::CULL_FRONT);
			break;
		default:
			break;
		}
		
		// Debug�� Object Render
		m_DebugRenderObj->MeshRender()->GetMaterial(0)->SetScalarParam(INT_0, iter->Shape);
		m_DebugRenderObj->MeshRender()->GetMaterial(0)->SetScalarParam(VEC4_0, iter->Color);

		// Depth ���� ���ο� ���� ���� �׽�Ʈ ���� üũ
		if (iter->DepthTest)
		{
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetDSType(DS_TYPE::NO_WRITE);
		}
		else
		{
			m_DebugRenderObj->MeshRender()->GetMaterial(0)->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
		}

		m_DebugRenderObj->Render();

		iter->Age += DT_Engine;

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