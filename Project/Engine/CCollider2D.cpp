#include "pch.h"
#include "CCollider2D.h"

#include "CTransform.h"
#include "CScript.h"

#include "CTaskManager.h"

bool CCollider2D::m_IsShow(true);

CCollider2D::CCollider2D()
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_Scale(Vec3(1.f, 1.f, 1.f))
	, m_Absolute(false)
	, m_Active(true)
	, m_SemiDeactive(false)
{
}

CCollider2D::CCollider2D(const CCollider2D& other)
	: CComponent(other)
	, m_Offset(other.m_Offset)
	, m_FinalPos(other.m_FinalPos)
	, m_Scale(other.m_Scale)
	, m_Rotation(other.m_Rotation)
	, m_Absolute(other.m_Absolute)
	, m_matColWorld(other.m_matColWorld)
	, m_OverlapCount(0)
	, m_Active(other.m_Active)
	, m_SemiDeactive(false)
{
}	

CCollider2D::~CCollider2D()
{

}

void CCollider2D::Activate()
{
	m_Active = true;
}

// First Frame
// Collider -> Final Tick Deactive() -> AddTask
// Collision :: Semi = false, Active = true
// Task Manager :: Semi = true, Active = true

// Second Frame
// Collider :: Semi true -> AddTask
// Collision :: Semi = true -> End Overlap
// Task Manager :: Semi = false, Active = false

// Third Frame
// Collider :: Active = false -> Collider Deactive
// Collision :: Collision Deactive

void CCollider2D::Deactivate()
{
	// 2�ܰ�� ������ ��Ȱ��ȭ ����
	// 1. COLLIDER2D_SEMI_DEACTIVE
	// 2. COLLIDER2D_DEACTIVE
	tTask task = {};
	task.Type = TASK_TYPE::COLLIDER2D_SEMI_DEACTIVE;
	task.dwParam_0 = (DWORD_PTR)this;
	CTaskManager::GetInst()->AddTask(task);
}

void CCollider2D::FinalTick()
{
	// ��Ȱ��ȭ ���� ���¸�, ��Ȱ��ȭ ��Ų��.
	if (m_SemiDeactive)
	{
		CTaskManager::GetInst()->AddTask(tTask{ TASK_TYPE::COLLIDER2D_DEACTIVE, (DWORD_PTR)this });
	}
	// ��Ȱ��ȭ ���¶�� �浹ü ��� ���� X
	if (!m_Active)
		return;

	// ���� ��ġ ���� ���
	Vec3 vObjectPos = Transform()->GetRelativePos();
	m_FinalPos = vObjectPos + m_Offset;

	if (m_Absolute == false)
	{
		Matrix matScale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		Matrix matRot = XMMatrixRotationZ(m_Rotation.z);
		Matrix matTrans = XMMatrixTranslation(m_Offset.x, m_Offset.y, m_Offset.z);

		// Collider ���� ������� ũ��, ȸ��, ��ġ
		m_matColWorld = matScale * matRot * matTrans;

		// ������Ʈ�� ���� ����� ���������� ���� ( ������� ���� �����ϸ鼭 ���� ������ )
		m_matColWorld *= Transform()->GetWorldMat();
	}
	else
	{
		// Collider �� �������� ũ�� ȸ�� ��ġ�� �����ֱ�
		Matrix matScale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		Matrix matRot = XMMatrixRotationZ(m_Rotation.z);
		Matrix matTrans = XMMatrixTranslation(m_Offset.x, m_Offset.y, m_Offset.z);

		// Collider ���� ������� ũ��, ȸ��, ��ġ
		m_matColWorld = matScale * matRot * matTrans;

		Matrix matObjScaleInv = XMMatrixIdentity();
		Vec3 vObjScale = Transform()->GetRelativeScale();
		matObjScaleInv = XMMatrixScaling(vObjectPos.x, vObjectPos.y, vObjectPos.z);
		matObjScaleInv = XMMatrixInverse(nullptr, matObjScaleInv);

		// ������Ʈ�� ���� ����� ���������� ���� ( ������� ���� �����ϸ鼭 ���� ������ )
		m_matColWorld = m_matColWorld * matObjScaleInv * Transform()->GetWorldMat();
	}

	if (!m_IsShow)
		return;

	if (m_OverlapCount == 0)
		DrawDebugRect(m_matColWorld, Vec4(0.f, 1.f, 0.f, 1.f), 0.f);
	else if (m_OverlapCount >= 1)
		DrawDebugRect(m_matColWorld, Vec4(1.f, 0.2f, 0.2f, 1.f), 0.f);
	else
		assert(nullptr);
}

void CCollider2D::BeginOverlap(CCollider2D* otherCollider)
{
	++m_OverlapCount;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();

	for (size_t i = 0; i < vecScripts.size(); i++)
	{
		vecScripts[i]->BeginOverlap(this, otherCollider->GetOwner(), otherCollider);
	}
}

void CCollider2D::Overlap(CCollider2D* otherCollider)
{
	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();

	for (size_t i = 0; i < vecScripts.size(); i++)
	{
		vecScripts[i]->Overlap(this, otherCollider->GetOwner(), otherCollider);
	}
}

void CCollider2D::EndOverlap(CCollider2D* otherCollider)
{
	--m_OverlapCount;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();

	for (size_t i = 0; i < vecScripts.size(); i++)
	{
		vecScripts[i]->EndOverlap(this, otherCollider->GetOwner(), otherCollider);
	}
}

void CCollider2D::SaveToLevelFile(FILE* file)
{
	fwrite(&m_Offset, sizeof(Vec3), 1, file);
	fwrite(&m_Scale, sizeof(Vec3), 1, file);
	fwrite(&m_Rotation, sizeof(Vec3), 1, file);
	fwrite(&m_Absolute, sizeof(bool), 1, file);
	fwrite(&m_Active, sizeof(bool), 1, file);
	fwrite(&m_SemiDeactive, sizeof(bool), 1, file);
}

void CCollider2D::LoadFromLevelFile(FILE* file)
{
	fread(&m_Offset, sizeof(Vec3), 1, file);
	fread(&m_Scale, sizeof(Vec3), 1, file);
	fread(&m_Rotation, sizeof(Vec3), 1, file);
	fread(&m_Absolute, sizeof(bool), 1, file);
	fread(&m_Active, sizeof(bool), 1, file);
	fread(&m_SemiDeactive, sizeof(bool), 1, file);
}