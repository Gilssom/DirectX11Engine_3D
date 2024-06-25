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

	// ũ�� -> ȸ�� -> �̵� �������� ����ؾ� ��

	// 4���� ��� ���
	// ȸ�� ������ ũ��, �̵��� �Ѳ����� �ϱ� ���� ����� 3�� ����� ���� �̸� �������� �ع�����.
	Matrix matScale;

	if(m_IsRight)
		matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z); // ��� ��� �Լ� ����
	else if(m_IsLeft)
		matScale = XMMatrixScaling(-m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z); // ��� ��� �Լ� ����

	Matrix matRot =  XMMatrixRotationX(m_RelativeRotation.x);
		   matRot *= XMMatrixRotationY(m_RelativeRotation.y);
		   matRot *= XMMatrixRotationZ(m_RelativeRotation.z);

	Matrix matTranslation = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);

	m_matWorld = matScale * matRot * matTranslation;

	// ������Ʈ�� ���� ���� ���
	m_WorldDir[(UINT)DIR_TYPE::RIGHT]	= m_RelativeDir[(UINT)DIR_TYPE::RIGHT]	= XAxis;
	m_WorldDir[(UINT)DIR_TYPE::UP]		= m_RelativeDir[(UINT)DIR_TYPE::UP]		= YAxis;
	m_WorldDir[(UINT)DIR_TYPE::FRONT]	= m_RelativeDir[(UINT)DIR_TYPE::FRONT]	= ZAxis;

	for (int i = 0; i < 3; i++)
	{
		// ȸ�� ����� 4�� ��ǥ���� ������ ���� ������ 1�� �ص� ������		( Coord )
		// ���� ����� ��� ��ǥ�� ������ �ֱ� ������ 0���� ���־�� �Ѵ�.	( Normal )
		m_RelativeDir[i] = XMVector3TransformNormal(m_RelativeDir[i], matRot);
		m_RelativeDir[i].Normalize();
	}

	// �θ� �ֱ� ������ �θ���� ������� ��ǥ�� ����� ���־����
	if (GetOwner()->GetParent())
	{
		const Matrix& matParentWorldMat = GetOwner()->GetParent()->Transform()->GetWorldMat();

		// �������� ũ�⸦ ����� ���
		if (m_Absolute)
		{
			Vec3 vParentScale = GetOwner()->GetParent()->Transform()->GetWorldScale();

			// �θ� ũ���� �����
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
	// �θ� ������Ʈ�� ���� ���, ������ �ֻ��� �θ� ������Ʈ
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
	// ������ GPU �� ������
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);

	// �ڽ��� ����ִ� Camera �� View ��� ������ �´�.
	g_Trans.matWorld = m_matWorld;

	g_Trans.matWV = g_Trans.matWorld * g_Trans.matView;
	g_Trans.matWVP = g_Trans.matWV * g_Trans.matProj;

	pCB->SetData(&g_Trans);
	pCB->Binding();
}

Vec3 CTransform::GetWorldPos()
{
	// �̵� ��ǥ�� ����� 4�� ����
	return m_matWorld.Translation();
}

Vec3 CTransform::GetWorldScale()
{
	// ũ��� ȸ���� 3 x 3 ����� �����ؼ� �����
	CGameObject* pObject = GetOwner();
	Vec3 vWorldScale = Vec3(1.f, 1.f, 1.f);

	while (pObject)
	{
		// �θ��� Scale ���� �޾ƿ���
		vWorldScale *= pObject->Transform()->GetRelativeScale();

		// �θ��� Absulute �� ���� ������ ���� ����
		if (pObject->Transform()->IsAbsolute())
			break;

		// �θ��� �θ�� ����, �θ��� �θ� ���� ��� while�� �ڵ� ����
		pObject = pObject->GetParent();
	}

	// ���� ����� World Scale ��ȯ
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