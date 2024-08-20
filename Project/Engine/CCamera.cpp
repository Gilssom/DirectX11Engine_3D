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

#include "components.h"

#include "CDevice.h"

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
	m_vecDeferred.clear();
	m_vecDecal.clear();
	m_vecOpaque.clear();
	m_vecMasked.clear();
	m_vecTransParent.clear();
	m_vecParticle.clear();
	m_vecPostProcess.clear();
}

void CCamera::Render_deferred()
{
	for (size_t i = 0; i < m_vecDeferred.size(); i++)
	{
		m_vecDeferred[i]->Render();
		CRenderManager::GetInst()->AddDrawCall();
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
				// Render Com 있는지 확인 > Material 있는지 확인 > Shader 있는지 확인 후 처리
				if (!vecObjects[j]->GetRenderComponent() 
					|| vecObjects[j]->GetRenderComponent()->GetMesh() == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial() == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader() == nullptr)
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

				SHADER_DOMAIN domain = vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader()->GetDomain();

				switch (domain)
				{
				case SHADER_DOMAIN::DOMAIN_DEFERRED:
					m_vecDeferred.push_back(vecObjects[j]);					
					break;
				case SHADER_DOMAIN::DOMAIN_DECAL:
					m_vecDecal.push_back(vecObjects[j]);
					break;


				case SHADER_DOMAIN::DOMAIN_OPAQUE:
					m_vecOpaque.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_MASKED:
					m_vecMasked.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
					m_vecTransParent.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_PARTICLE:
					m_vecParticle.push_back(vecObjects[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
					m_vecPostProcess.push_back(vecObjects[j]);
					break;
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
					|| vecObjects[j]->GetRenderComponent()->GetMaterial() == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader() == nullptr
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