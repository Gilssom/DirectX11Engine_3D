#include "pch.h"
#include "CCamera.h"

#include "CKeyManager.h"

#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "CRenderManager.h"
#include "CMRT.h"
#include "CRenderComponent.h"
#include "CAnimator3D.h"

#include "components.h"

#include "CDevice.h"
#include "CInstancingBuffer.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_Frustum(this)
	, m_ProjType(PROJ_TYPE::PERSPECTIVE)
	, m_CamPriority(-1)
	, m_FOV((XM_PI) / 3.f)
	, m_Far(10000.f)
	, m_Width(0.f)
	, m_Scale(1.f)
	, m_LayerCheck(0)
{
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();
	m_Width = vRenderResol.x;
	m_AspectRatio = vRenderResol.x / vRenderResol.y;
}

CCamera::CCamera(const CCamera& other)
	: CComponent(other)
	, m_Frustum(other.m_Frustum)
	, m_ProjType(other.m_ProjType)
	, m_CamPriority(-1)
	, m_FOV(other.m_FOV)
	, m_Far(other.m_Far)
	, m_Width(other.m_Width)
	, m_Scale(other.m_Scale)
	, m_LayerCheck(other.m_LayerCheck)
{
	m_Frustum.SetOwner(this);
}

CCamera::~CCamera()
{

}

void CCamera::Begin()
{
	// Level 시작 시 Level 안의 Camera 들이 Render Manager 에 등록
	CRenderManager::GetInst()->RegisterCamera(this, m_CamPriority);
}

void CCamera::FinalTick()
{
	// View 행렬 계산
	Vec3 vCamWorldPos = Transform()->GetWorldPos();

	// 1. 이동 행렬 만들기
	// 오브젝트들은 카메라 이동량의 반대만큼 계산을 해주어야함
	Matrix matViewTrans = XMMatrixTranslation(-vCamWorldPos.x, -vCamWorldPos.y, -vCamWorldPos.z);

	// 2. 회전 행렬 만들기 ( 3 x 3 행렬 )
	// 만약 카메라가 월드 상의 Z 축과 동일하지 않다면?
	// ** 그만큼 카메라가 Z 축을 바라볼 수 있는 회전량을 회전 행렬로 구해야 한다. **
	
	// 자동적으로 이동 행렬에서 모든 물체를 카메라 기점으로 만들어 놓았기 때문에
	// 모든 물체는 카메라의 회전에 따라 다 같이 Z 축 기준으로 움직이게 된다.

	// 1 0 0  | X
	// 0 1 0  | Y
	// 0 0 1  | Z

	// 이동 행렬 x 회전 행렬 = 단위 행렬 (사칙연산의 항등원과 동일함)
	// 역원 : 자신의 항등원이 나오게 해주는 수

	// 그럼 카메라의 이동 행렬에 "무언가" 를 곱했을 때 단위행렬이 나온다?
	// 그럼 곱한 행렬은 카메라 이동 행렬의 "역행렬(역원)" 이 된다.

	// 각 행렬이 서로 90도를 이루고 있으면 직교 행렬이라 부른다.

	// 직교 행렬의 역행렬은 전치 행렬이다.
	// 전치 행렬이 곧 회전 행렬이 된다.

	// 카메라의 우, 상, 전 벡터
	Vec3 vR = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	Matrix matViewtRot = XMMatrixIdentity();

	matViewtRot._11 = vR.x;  matViewtRot._12 = vU.x;	matViewtRot._13 = vF.x;
	matViewtRot._21 = vR.y;  matViewtRot._22 = vU.y;	matViewtRot._23 = vF.y;
	matViewtRot._31 = vR.z;  matViewtRot._32 = vU.z;	matViewtRot._33 = vF.z;
	
	// 행렬은 교환 법칙 성립이 안된다.
	m_matView = matViewTrans * matViewtRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);


	// 3. Proj 행렬 계산 (투영) = 원근투영 | 직교투영
	if (m_ProjType == PROJ_TYPE::PERSPECTIVE)
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
	}
	else
	{
		// 직교 투영은 카메라의 시야각이 필요없음
		// 일직선으로 투영을 할 것이기 때문에 투영할 가로 세로 길이만 필요함
		m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale, 1.f, m_Far);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);

	// 마우스 방향 Ray 계산
	CalcRay();

	// 절두체 계산
	m_Frustum.FinalTick();
}

void CCamera::Render()
{
	// == Render Manager 로 모두 이동 ==
}

void CCamera::CalcRay()
{
	// ViewPort 정보
	CMRT* pSwapChainMRT = CRenderManager::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);

	if (pSwapChainMRT == nullptr)
		return;

	const D3D11_VIEWPORT& VP = pSwapChainMRT->GetViewPort();

	// 현재 마우스 좌표
	Vec2 vMousePos = CKeyManager::GetInst()->GetMousePos();

	// 마우스를 향하는 직선은 카메라 위치를 지난다.
	m_Ray.vStart = Transform()->GetWorldPos();

	// View 공간 상에서 카메라에서 마우스 방향을 향하는 방향벡터를 구한다.
	//  - 마우스가 있는 좌표를 -1 ~ 1 사이의 정규화된 좌표로 바꾼다.
	//  - 투영행렬의 _11, _22 에 있는 값은 Near 평면상에서 Near 값을 가로 세로 길이로 나눈값
	//  - 실제 ViewSpace 상에서의 Near 평명상에서 마우스가 있는 지점을 향하는 위치를 구하기 위해서 비율을 나누어서 
	//  - 실제 Near 평면상에서 마우스가 향하는 위치를 좌표를 구함
	m_Ray.vDir.x = (((vMousePos.x - VP.TopLeftX) * 2.f / VP.Width) - 1.f) / m_matProj._11;
	m_Ray.vDir.y = -(((vMousePos.y - VP.TopLeftY) * 2.f / VP.Height) - 1.f) / m_matProj._22;
	m_Ray.vDir.z = 1.f;

	// 방향 벡터에 ViewMatInv 를 적용, 월드상에서의 방향을 알아낸다.
	m_Ray.vDir = XMVector3TransformNormal(m_Ray.vDir, m_matViewInv);
	m_Ray.vDir.Normalize();
}

void CCamera::SortClear()
{
	// 이전 프레임 분류 정보 제거
	m_mapInstGroup_D.clear();
	m_mapInstGroup_F.clear();

	m_vecDecal.clear();
	m_vecOpaque.clear();
	m_vecMasked.clear();
	m_vecTransParent.clear();
	m_vecParticle.clear();
	m_vecPostProcess.clear();
}

void CCamera::Render_deferred()
{
	for (auto& pair : m_mapSingleObject)
	{
		pair.second.clear();
	}

	// Deferred object render
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;

		// instancing 개수 조건 미만이거나
		// Animation2D 오브젝트거나(스프라이트 애니메이션 오브젝트)
		// Shader 가 Instancing 을 지원하지 않는경우
		if (pair.second.size() <= 10
			|| pair.second[0].pObject->Animator2D()
			|| pair.second[0].pObject->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// 해당 물체들은 단일 랜더링으로 전환
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObject>>::iterator iter
					= m_mapSingleObject.find((INT_PTR)pair.second[i].pObject);

				if (iter != m_mapSingleObject.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObject.insert(make_pair((INT_PTR)pair.second[i].pObject, vector<tInstObject>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObject;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx);

		// Instancing 버퍼 클리어
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObject->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObject->Animator3D())
			{
				pair.second[i].pObject->Animator3D()->Binding();
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObject->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh 알리기
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->Binding_Inst();
		pMesh->Render_Instancing(pair.second[0].iMtrlIdx);
		CRenderManager::GetInst()->AddDrawCall();

		// 정리
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh 알리기
			pMtrl->SetBoneCount(0);
		}
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObject)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObject->Transform()->Binding();

		for (auto& instObj : pair.second)
		{
			instObj.pObject->GetRenderComponent()->Render(instObj.iMtrlIdx);
			CRenderManager::GetInst()->AddDrawCall();
		}

		if (pair.second[0].pObject->Animator3D())
		{
			pair.second[0].pObject->Animator3D()->ClearData();
		}
	}
}

void CCamera::Render_decal()
{
	for (size_t i = 0; i < m_vecDecal.size(); i++)
	{
		m_vecDecal[i]->Render();
		CRenderManager::GetInst()->AddDrawCall();
	}
}

void CCamera::Render_opaque()
{
	for (size_t i = 0; i < m_vecOpaque.size(); i++)
	{
		m_vecOpaque[i]->Render();
		CRenderManager::GetInst()->AddDrawCall();
	}

	// m_mapInstGroup_F 에 있는 객체들에 대한 렌더링 처리 추가
	for (auto& pair : m_mapInstGroup_F)
	{
		CGameObject* pObj = pair.second[0].pObject;
		pObj->Render();
		//Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		//Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx);
		//
		//pMtrl->Binding_Inst();
		//pMesh->Render_Instancing(pair.second[0].iMtrlIdx);
		//CRenderManager::GetInst()->AddDrawCall();
	}
}

void CCamera::Render_masked()
{
	for (size_t i = 0; i < m_vecMasked.size(); i++)
	{
		m_vecMasked[i]->Render();
		CRenderManager::GetInst()->AddDrawCall();
	}
}

void CCamera::Render_transparent()
{
	for (size_t i = 0; i < m_vecTransParent.size(); i++)
	{
		m_vecTransParent[i]->Render();
		CRenderManager::GetInst()->AddDrawCall();
	}
}

void CCamera::Render_particle()
{
	for (size_t i = 0; i < m_vecParticle.size(); i++)
	{
		m_vecParticle[i]->Render();
		CRenderManager::GetInst()->AddDrawCall();
	}
}

void CCamera::Render_postprocess()
{
	for (size_t i = 0; i < m_vecPostProcess.size(); i++)
	{
		// Post Process 가 여러개여도 누적되어서 사용할 수 있음
		CRenderManager::GetInst()->CopyRenderTarget();

		m_vecPostProcess[i]->Render();
		CRenderManager::GetInst()->AddDrawCall();
	}
}

void CCamera::Render_shadowmap()
{
	for (size_t i = 0; i < m_vecShadowMap.size(); i++)
	{
		m_vecShadowMap[i]->GetRenderComponent()->Render_shadowmap();
	}
}

void CCamera::SetCameraPriority(int priority)
{
	// 카메라 우선순위 설정
	m_CamPriority = priority;
}

void CCamera::LayerCheck(int layerIdx)
{
	if (m_LayerCheck & (1 << layerIdx))
	{
		m_LayerCheck &= ~(1 << layerIdx);
	}
	else
	{
		m_LayerCheck |= (1 << layerIdx);
	}
}

void CCamera::SortObject()
{
	SortClear();

	// Shader 의 속성마다 렌더링이 달라지기 때문에 오브젝트 정렬이 필요하다.
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// 해당 i번째 Layer 에 비트 체크가 되어 있는지 확인
		if (m_LayerCheck & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

			for (size_t j = 0; j < vecObjects.size(); j++)
			{
				CRenderComponent* pRenderCom = vecObjects[j]->GetRenderComponent();

				// Rendering 기능이 없는 오브젝트는 제외
				if(pRenderCom == nullptr || pRenderCom->GetMesh() == nullptr)
					continue;

				// Frustum Check 기능을 사용하는지, 사용한다면 Frsutum 내부에 들어오는지 Check
				if (vecObjects[j]->GetRenderComponent()->IsFrustumCheck())
				{
					// vecObjects[j] 의 Bounding Box 에 대한 Check 가 필요하다.
					if (vecObjects[j]->BoundingBox())
					{
						// Bounding Box 를 기준으로 비교
						Vec3 vWorldPos = vecObjects[j]->BoundingBox()->GetWorldPos();
						float radius = vecObjects[j]->BoundingBox()->GetRadius();

						// Frustum 내부에 해당 object 가 없다면 continue
						if (m_Frustum.FrustumSphereCheck(vWorldPos, radius) == false)
						{
							continue;
						}
					}
					else
					{
						// 원점을 기준으로 비교
						Vec3 vWorldPos = vecObjects[j]->Transform()->GetWorldPos();
						Vec3 vWorldScale = vecObjects[j]->Transform()->GetWorldScale();

						float radius = vWorldScale.x;
						if (radius < vWorldScale.y) radius = vWorldScale.y;
						if (radius < vWorldScale.z) radius = vWorldScale.z;

						// Frustum 내부에 해당 object 가 없다면 continue
						if (m_Frustum.FrustumSphereCheck(vWorldPos, radius) == false)
						{
							continue;
						}
					}
				}

				
				// 메테리얼 개수만큼 반복
				UINT iMtrlCount = pRenderCom->GetMaterialCount();

				for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
				{
					// 재질이 없거나, 재질의 쉐이더가 설정이 안된 경우
					if (nullptr == pRenderCom->GetMaterial(iMtrl)
						|| nullptr == pRenderCom->GetMaterial(iMtrl)->GetShader())
					{
						continue;
					}

					// 쉐이더 도메인에 따른 분류
					SHADER_DOMAIN eDomain = pRenderCom->GetMaterial(iMtrl)->GetShader()->GetDomain();
					Ptr<CGraphicShader> pShader = pRenderCom->GetMaterial(iMtrl)->GetShader();

					switch (eDomain)
					{
					case SHADER_DOMAIN::DOMAIN_DEFERRED:
					case SHADER_DOMAIN::DOMAIN_OPAQUE:
					case SHADER_DOMAIN::DOMAIN_MASKED:
					{
						// Shader 의 DOMAIN 에 따라서 인스턴싱 그룹을 분류한다.
						map<ULONG64, vector<tInstObject>>* pMap = NULL;
						Ptr<CMaterial> pMtrl = pRenderCom->GetMaterial(iMtrl);

						if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED)
						{
							pMap = &m_mapInstGroup_D;
						}
						else if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_OPAQUE
							  || pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_MASKED)
						{
							pMap = &m_mapInstGroup_F;
						}
						else
						{
							assert(nullptr);
							continue;
						}

						uInstID uID = {};
						uID.LLID = pRenderCom->GetInstID(iMtrl);

						// ID 가 0 다 ==> Mesh 나 Material 이 셋팅되지 않았다.
						if (0 == uID.LLID)
							continue;

						map<ULONG64, vector<tInstObject>>::iterator iter = pMap->find(uID.LLID);
						if (iter == pMap->end())
						{
							pMap->insert(make_pair(uID.LLID, vector<tInstObject>{tInstObject{ vecObjects[j], iMtrl }}));
						}
						else
						{
							iter->second.push_back(tInstObject{ vecObjects[j], iMtrl });
						}
					}
					break;
					case SHADER_DOMAIN::DOMAIN_DECAL:
						m_vecDecal.push_back(vecObjects[j]);
						break;
					case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
						m_vecTransParent.push_back(vecObjects[j]);
						break;
					case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
						m_vecPostProcess.push_back(vecObjects[j]);
						break;
					}
				}
			}
		}
	}
}

void CCamera::SortObject_ShadowMap()
{
	m_vecShadowMap.clear();

	// Shader 의 속성마다 렌더링이 달라지기 때문에 오브젝트 정렬이 필요하다.
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// 해당 i번째 Layer 에 비트 체크가 되어 있는지 확인
		if (m_LayerCheck & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

			for (size_t j = 0; j < vecObjects.size(); j++)
			{
				// Render Com 있는지 확인 > Material 있는지 확인 > Shader 있는지 확인 후 처리
				if (!vecObjects[j]->GetRenderComponent()
					|| vecObjects[j]->GetRenderComponent()->GetMesh() == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial(0) == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial(0)->GetShader() == nullptr
					|| vecObjects[j]->GetRenderComponent()->IsDynamicShadow() == false)
					continue;

				//// Frustum Check 기능을 사용하는지, 사용한다면 Frsutum 내부에 들어오는지 Check
				//if (vecObjects[j]->GetRenderComponent()->IsFrustumCheck())
				//{
				//	// vecObjects[j] 의 Bounding Box 에 대한 Check 가 필요하다.
				//	if (vecObjects[j]->BoundingBox())
				//	{
				//		// Bounding Box 를 기준으로 비교
				//		Vec3 vWorldPos = vecObjects[j]->BoundingBox()->GetWorldPos();
				//		float radius = vecObjects[j]->BoundingBox()->GetRadius();

				//		// Frustum 내부에 해당 object 가 없다면 continue
				//		if (m_Frustum.FrustumSphereCheck(vWorldPos, radius) == false)
				//		{
				//			continue;
				//		}
				//	}
				//	else
				//	{
				//		// 원점을 기준으로 비교
				//		Vec3 vWorldPos = vecObjects[j]->Transform()->GetWorldPos();
				//		Vec3 vWorldScale = vecObjects[j]->Transform()->GetWorldScale();

				//		float radius = vWorldScale.x;
				//		if (radius < vWorldScale.y) radius = vWorldScale.y;
				//		if (radius < vWorldScale.z) radius = vWorldScale.z;

				//		// Frustum 내부에 해당 object 가 없다면 continue
				//		if (m_Frustum.FrustumSphereCheck(vWorldPos, radius) == false)
				//		{
				//			continue;
				//		}
				//	}
				//}

				m_vecShadowMap.push_back(vecObjects[j]);
			}
		}
	}
}

void CCamera::SaveToLevelFile(FILE* file)
{
	fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, file);
	fwrite(&m_CamPriority, sizeof(int), 1, file);
	fwrite(&m_FOV, sizeof(float), 1, file);
	fwrite(&m_Far, sizeof(float), 1, file);
	fwrite(&m_Width, sizeof(float), 1, file);
	fwrite(&m_AspectRatio, sizeof(float), 1, file);
	fwrite(&m_Scale, sizeof(float), 1, file);
	fwrite(&m_LayerCheck, sizeof(UINT), 1, file);
}

void CCamera::LoadFromLevelFile(FILE* file)
{
	fread(&m_ProjType, sizeof(PROJ_TYPE), 1, file);
	fread(&m_CamPriority, sizeof(int), 1, file);
	fread(&m_FOV, sizeof(float), 1, file);
	fread(&m_Far, sizeof(float), 1, file);
	fread(&m_Width, sizeof(float), 1, file);
	fread(&m_AspectRatio, sizeof(float), 1, file);
	fread(&m_Scale, sizeof(float), 1, file);
	fread(&m_LayerCheck, sizeof(UINT), 1, file);

	//SetCameraPriority(m_CamPriority);
}