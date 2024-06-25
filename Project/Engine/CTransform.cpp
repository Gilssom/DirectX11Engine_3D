#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_RelativeScale(Vec3(1.f, 1.f, 1.f))
	, m_Absolute(false)
	, m_IsRight(true)
	, m_IsLeft(false)
{

}

CTransform::~CTransform()
{

}

void CTransform::FinalTick()
{
	m_matWorld = XMMatrixIdentity();

	// 크기 -> 회전 -> 이동 서순으로 계산해야 함

	// 4차원 행렬 사용
	// 회전 각도와 크기, 이동을 한꺼번에 하기 위해 행렬을 3개 만들어 놓고 미리 곱연산을 해버린다.
	Matrix matScale;

	if(m_IsRight)
		matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z); // 행렬 계산 함수 지원
	else if(m_IsLeft)
		matScale = XMMatrixScaling(-m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z); // 행렬 계산 함수 지원

	Matrix matRot =  XMMatrixRotationX(m_RelativeRotation.x);
		   matRot *= XMMatrixRotationY(m_RelativeRotation.y);
		   matRot *= XMMatrixRotationZ(m_RelativeRotation.z);

	Matrix matTranslation = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);

	m_matWorld = matScale * matRot * matTranslation;

	// 오브젝트의 방향 정보 계산
	m_WorldDir[(UINT)DIR_TYPE::RIGHT]	= m_RelativeDir[(UINT)DIR_TYPE::RIGHT]	= XAxis;
	m_WorldDir[(UINT)DIR_TYPE::UP]		= m_RelativeDir[(UINT)DIR_TYPE::UP]		= YAxis;
	m_WorldDir[(UINT)DIR_TYPE::FRONT]	= m_RelativeDir[(UINT)DIR_TYPE::FRONT]	= ZAxis;

	for (int i = 0; i < 3; i++)
	{
		// 회전 행렬의 4차 좌표에는 정보가 없기 때문에 1도 해도 되지만		( Coord )
		// 월드 행렬은 모든 좌표를 가지고 있기 때문에 0으로 해주어야 한다.	( Normal )
		m_RelativeDir[i] = XMVector3TransformNormal(m_RelativeDir[i], matRot);
		m_RelativeDir[i].Normalize();
	}

	// 부모가 있기 때문에 부모와의 상대적인 좌표로 계산을 해주어야함
	if (GetOwner()->GetParent())
	{
		const Matrix& matParentWorldMat = GetOwner()->GetParent()->Transform()->GetWorldMat();

		// 절대적은 크기를 사용할 경우
		if (m_Absolute)
		{
			Vec3 vParentScale = GetOwner()->GetParent()->Transform()->GetWorldScale();

			// 부모 크기의 역행렬
			Matrix matScaleInv = XMMatrixInverse(nullptr, XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z));

			m_matWorld = m_matWorld * matScaleInv * matParentWorldMat;
		}
		else
		{
			m_matWorld *= matParentWorldMat;
		}

		for (int i = 0; i < 3; i++)
		{
			m_WorldDir[i] = XMVector3TransformNormal(m_WorldDir[i], m_matWorld);
			m_WorldDir[i].Normalize();
		}
	}
	// 부모 오브젝트가 없는 경우, 본인이 최상위 부모 오브젝트
	else
	{
		for (int i = 0; i < 3; i++)
		{
			m_WorldDir[i] = m_RelativeDir[i];
		}
	}
}

void CTransform::Binding()
{
	// 데이터 GPU 로 보내기
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);

	// 자신을 찍고있는 Camera 의 View 행렬 정보가 온다.
	g_Trans.matWorld = m_matWorld;

	g_Trans.matWV = g_Trans.matWorld * g_Trans.matView;
	g_Trans.matWVP = g_Trans.matWV * g_Trans.matProj;

	pCB->SetData(&g_Trans);
	pCB->Binding();
}

Vec3 CTransform::GetWorldPos()
{
	// 이동 좌표는 행렬의 4행 고정
	return m_matWorld.Translation();
}

Vec3 CTransform::GetWorldScale()
{
	// 크기와 회전은 3 x 3 행렬을 공유해서 사용함
	CGameObject* pObject = GetOwner();
	Vec3 vWorldScale = Vec3(1.f, 1.f, 1.f);

	while (pObject)
	{
		// 부모의 Scale 정보 받아오기
		vWorldScale *= pObject->Transform()->GetRelativeScale();

		// 부모의 Absulute 가 켜져 있으면 연산 중지
		if (pObject->Transform()->IsAbsolute())
			break;

		// 부모의 부모로 접근, 부모의 부모가 없을 경우 while문 자동 중지
		pObject = pObject->GetParent();
	}

	// 최종 연산된 World Scale 반환
	return vWorldScale;
}

Vec3 CTransform::GetWorldRotation()
{
	return Vec3();
}

void CTransform::SaveToLevelFile(FILE* file)
{
	fwrite(&m_RelativePos, sizeof(Vec3), 1, file);
	fwrite(&m_RelativeScale, sizeof(Vec3), 1, file);
	fwrite(&m_RelativeRotation, sizeof(Vec3), 1, file);
	fwrite(&m_Absolute, sizeof(bool), 1, file);
}

void CTransform::LoadFromLevelFile(FILE* file)
{
	fread(&m_RelativePos, sizeof(Vec3), 1, file);
	fread(&m_RelativeScale, sizeof(Vec3), 1, file);
	fread(&m_RelativeRotation, sizeof(Vec3), 1, file);
	fread(&m_Absolute, sizeof(bool), 1, file);
}