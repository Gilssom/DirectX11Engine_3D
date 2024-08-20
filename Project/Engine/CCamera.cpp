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
	// Level ���� �� Level ���� Camera ���� Render Manager �� ���
	CRenderManager::GetInst()->RegisterCamera(this, m_CamPriority);
}

void CCamera::FinalTick()
{
	// View ��� ���
	Vec3 vCamWorldPos = Transform()->GetWorldPos();

	// 1. �̵� ��� �����
	// ������Ʈ���� ī�޶� �̵����� �ݴ븸ŭ ����� ���־����
	Matrix matViewTrans = XMMatrixTranslation(-vCamWorldPos.x, -vCamWorldPos.y, -vCamWorldPos.z);

	// 2. ȸ�� ��� ����� ( 3 x 3 ��� )
	// ���� ī�޶� ���� ���� Z ��� �������� �ʴٸ�?
	// ** �׸�ŭ ī�޶� Z ���� �ٶ� �� �ִ� ȸ������ ȸ�� ��ķ� ���ؾ� �Ѵ�. **
	
	// �ڵ������� �̵� ��Ŀ��� ��� ��ü�� ī�޶� �������� ����� ���ұ� ������
	// ��� ��ü�� ī�޶��� ȸ���� ���� �� ���� Z �� �������� �����̰� �ȴ�.

	// 1 0 0  | X
	// 0 1 0  | Y
	// 0 0 1  | Z

	// �̵� ��� x ȸ�� ��� = ���� ��� (��Ģ������ �׵���� ������)
	// ���� : �ڽ��� �׵���� ������ ���ִ� ��

	// �׷� ī�޶��� �̵� ��Ŀ� "����" �� ������ �� ��������� ���´�?
	// �׷� ���� ����� ī�޶� �̵� ����� "�����(����)" �� �ȴ�.

	// �� ����� ���� 90���� �̷�� ������ ���� ����̶� �θ���.

	// ���� ����� ������� ��ġ ����̴�.
	// ��ġ ����� �� ȸ�� ����� �ȴ�.

	// ī�޶��� ��, ��, �� ����
	Vec3 vR = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	Matrix matViewtRot = XMMatrixIdentity();

	matViewtRot._11 = vR.x;  matViewtRot._12 = vU.x;	matViewtRot._13 = vF.x;
	matViewtRot._21 = vR.y;  matViewtRot._22 = vU.y;	matViewtRot._23 = vF.y;
	matViewtRot._31 = vR.z;  matViewtRot._32 = vU.z;	matViewtRot._33 = vF.z;
	
	// ����� ��ȯ ��Ģ ������ �ȵȴ�.
	m_matView = matViewTrans * matViewtRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);


	// 3. Proj ��� ��� (����) = �������� | ��������
	if (m_ProjType == PROJ_TYPE::PERSPECTIVE)
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
	}
	else
	{
		// ���� ������ ī�޶��� �þ߰��� �ʿ����
		// ���������� ������ �� ���̱� ������ ������ ���� ���� ���̸� �ʿ���
		m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale, 1.f, m_Far);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);

	// ���콺 ���� Ray ���
	CalcRay();

	// ����ü ���
	m_Frustum.FinalTick();
}

void CCamera::Render()
{
	// == Render Manager �� ��� �̵� ==
}

void CCamera::CalcRay()
{
	// ViewPort ����
	CMRT* pSwapChainMRT = CRenderManager::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	const D3D11_VIEWPORT& VP = pSwapChainMRT->GetViewPort();

	// ���� ���콺 ��ǥ
	Vec2 vMousePos = CKeyManager::GetInst()->GetMousePos();

	// ���콺�� ���ϴ� ������ ī�޶� ��ġ�� ������.
	m_Ray.vStart = Transform()->GetWorldPos();

	// View ���� �󿡼� ī�޶󿡼� ���콺 ������ ���ϴ� ���⺤�͸� ���Ѵ�.
	//  - ���콺�� �ִ� ��ǥ�� -1 ~ 1 ������ ����ȭ�� ��ǥ�� �ٲ۴�.
	//  - ��������� _11, _22 �� �ִ� ���� Near ���󿡼� Near ���� ���� ���� ���̷� ������
	//  - ���� ViewSpace �󿡼��� Near ���󿡼� ���콺�� �ִ� ������ ���ϴ� ��ġ�� ���ϱ� ���ؼ� ������ ����� 
	//  - ���� Near ���󿡼� ���콺�� ���ϴ� ��ġ�� ��ǥ�� ����
	m_Ray.vDir.x = (((vMousePos.x - VP.TopLeftX) * 2.f / VP.Width) - 1.f) / m_matProj._11;
	m_Ray.vDir.y = -(((vMousePos.y - VP.TopLeftY) * 2.f / VP.Height) - 1.f) / m_matProj._22;
	m_Ray.vDir.z = 1.f;

	// ���� ���Ϳ� ViewMatInv �� ����, ����󿡼��� ������ �˾Ƴ���.
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
		// Post Process �� ���������� �����Ǿ ����� �� ����
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
	// ī�޶� �켱���� ����
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

	// Shader �� �Ӽ����� �������� �޶����� ������ ������Ʈ ������ �ʿ��ϴ�.
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// �ش� i��° Layer �� ��Ʈ üũ�� �Ǿ� �ִ��� Ȯ��
		if (m_LayerCheck & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

			for (size_t j = 0; j < vecObjects.size(); j++)
			{
				// Render Com �ִ��� Ȯ�� > Material �ִ��� Ȯ�� > Shader �ִ��� Ȯ�� �� ó��
				if (!vecObjects[j]->GetRenderComponent() 
					|| vecObjects[j]->GetRenderComponent()->GetMesh() == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial() == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader() == nullptr)
					continue;

				// Frustum Check ����� ����ϴ���, ����Ѵٸ� Frsutum ���ο� �������� Check
				if (vecObjects[j]->GetRenderComponent()->IsFrustumCheck())
				{
					// vecObjects[j] �� Bounding Box �� ���� Check �� �ʿ��ϴ�.
					if (vecObjects[j]->BoundingBox())
					{
						// Bounding Box �� �������� ��
						Vec3 vWorldPos = vecObjects[j]->BoundingBox()->GetWorldPos();
						float radius = vecObjects[j]->BoundingBox()->GetRadius();

						// Frustum ���ο� �ش� object �� ���ٸ� continue
						if (m_Frustum.FrustumSphereCheck(vWorldPos, radius) == false)
						{
							continue;
						}
					}
					else
					{
						// ������ �������� ��
						Vec3 vWorldPos = vecObjects[j]->Transform()->GetWorldPos();
						Vec3 vWorldScale = vecObjects[j]->Transform()->GetWorldScale();

						float radius = vWorldScale.x;
						if (radius < vWorldScale.y) radius = vWorldScale.y;
						if (radius < vWorldScale.z) radius = vWorldScale.z;

						// Frustum ���ο� �ش� object �� ���ٸ� continue
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

	// Shader �� �Ӽ����� �������� �޶����� ������ ������Ʈ ������ �ʿ��ϴ�.
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// �ش� i��° Layer �� ��Ʈ üũ�� �Ǿ� �ִ��� Ȯ��
		if (m_LayerCheck & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

			for (size_t j = 0; j < vecObjects.size(); j++)
			{
				// Render Com �ִ��� Ȯ�� > Material �ִ��� Ȯ�� > Shader �ִ��� Ȯ�� �� ó��
				if (!vecObjects[j]->GetRenderComponent()
					|| vecObjects[j]->GetRenderComponent()->GetMesh() == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial() == nullptr
					|| vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader() == nullptr
					|| vecObjects[j]->GetRenderComponent()->IsDynamicShadow() == false)
					continue;

				//// Frustum Check ����� ����ϴ���, ����Ѵٸ� Frsutum ���ο� �������� Check
				//if (vecObjects[j]->GetRenderComponent()->IsFrustumCheck())
				//{
				//	// vecObjects[j] �� Bounding Box �� ���� Check �� �ʿ��ϴ�.
				//	if (vecObjects[j]->BoundingBox())
				//	{
				//		// Bounding Box �� �������� ��
				//		Vec3 vWorldPos = vecObjects[j]->BoundingBox()->GetWorldPos();
				//		float radius = vecObjects[j]->BoundingBox()->GetRadius();

				//		// Frustum ���ο� �ش� object �� ���ٸ� continue
				//		if (m_Frustum.FrustumSphereCheck(vWorldPos, radius) == false)
				//		{
				//			continue;
				//		}
				//	}
				//	else
				//	{
				//		// ������ �������� ��
				//		Vec3 vWorldPos = vecObjects[j]->Transform()->GetWorldPos();
				//		Vec3 vWorldScale = vecObjects[j]->Transform()->GetWorldScale();

				//		float radius = vWorldScale.x;
				//		if (radius < vWorldScale.y) radius = vWorldScale.y;
				//		if (radius < vWorldScale.z) radius = vWorldScale.z;

				//		// Frustum ���ο� �ش� object �� ���ٸ� continue
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