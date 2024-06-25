#include "pch.h"
#include "CCollisionManager.h"

#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CCollider2D.h"

CCollisionManager::CCollisionManager()
	: m_Matrix{}
{

}

CCollisionManager::~CCollisionManager()
{

}

void CCollisionManager::Tick()
{
	// ���� Level �� �����Ǿ� ���� �ʴٸ� return
	if (!CLevelManager::GetInst()->GetCurrentLevel())
		return;

	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Col = Row; Col < MAX_LAYER; ++Col)
		{
			// �ش� ĭ ��Ʈ üũ ���� �Ǵ�
			if (m_Matrix[Row] & (1 << Col))
			{
				CollisionBtwLayer(Row, Col);
			}
		}
	}
}

void CCollisionManager::LayerCheck(UINT layerLeftIndex, UINT layerRightIndex)
{
	UINT Row = layerLeftIndex;
	UINT Col = layerRightIndex;

	// ���� ���� ��, ���� ū ������
	if (Col < Row)
	{
		Row = layerRightIndex;
		Col = layerLeftIndex;
	}

	// ��Ʈ ������
	UINT CheckBit = (1 << Col);

	// ��Ʈ ���ϱ�
	m_Matrix[Row] |= CheckBit;

	//// ���� �ش� ��Ʈ�� ���� �ִٸ� ( Check Box On , Off ��� )
	//if (m_Matrix[Row] & CheckBit)
	//{
	//	// ��Ʈ ����
	//	m_Matrix[Row] &= ~CheckBit;
	//}
	//else
	//{
	//	// ��Ʈ ���ϱ�
	//	m_Matrix[Row] |= CheckBit;
	//}
}

void CCollisionManager::CollisionBtwLayer(UINT left, UINT right)
{
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

	CLayer* pLeftLayer = pCurLevel->GetLayer(left);
	CLayer* pRightLayer = pCurLevel->GetLayer(right);

	const vector<CGameObject*>& vecLeft = pLeftLayer->GetObjects();
	const vector<CGameObject*>& vecRight = pRightLayer->GetObjects();

	// ���� �ٸ� ���̾�� �浹 �˻�
	if (left != right)
	{
		for (size_t i = 0; i < vecLeft.size(); i++)
		{
			if (vecLeft[i]->Collider2D() == nullptr)
				continue;

			for (size_t j = 0; j < vecRight.size(); j++)
			{
				if (vecRight[j]->Collider2D() == nullptr)
					continue;

				CollisionBtwCollider2D(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D());
			}
		}
	}
	// ������ ���̾� ������ �浹 �˻�
	else
	{
		for (size_t i = 0; i < vecLeft.size(); i++)
		{
			if (vecLeft[i]->Collider2D() == nullptr)
				continue;

			// ������ �����̹Ƿ� ������ ��ȣ������ �̹� �ߴ� ��ȣ������ �˻�� �ʿ� ����
			for (size_t j = i + 1; j < vecRight.size(); j++)
			{
				if (vecRight[j]->Collider2D() == nullptr)
					continue;

				CollisionBtwCollider2D(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D());
			}
		}
	}
}

void CCollisionManager::CollisionBtwCollider2D(CCollider2D* leftCol, CCollider2D* rightCol)
{
	COL_ID id;
	id.LeftID = leftCol->GetID();
	id.RightID = rightCol->GetID();

	map<LONGLONG, bool>::iterator iter = m_ColInfo.find(id.ID);

	// �� ID �� ���� ������ ��ü�� �浹�� ���� ����.
	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	// �� ��ü �� �ϳ��� ���� ������ ���
	bool IsDead = leftCol->GetOwner()->IsDead() || rightCol->GetOwner()->IsDead();
	
	// �� ��ü �� �ϳ��� ��Ȱ��ȭ�� ���
	bool IsActive = leftCol->IsActive() && rightCol->IsActive();

	// �� ��ü �� �ϳ��� ��Ȱ��ȭ ������ ���
	bool IsSemiDeactive = leftCol->IsSemiDeactive() || rightCol->IsSemiDeactive();

	// �� ��ü �� �ϳ��� ��Ȱ��ȭ�� �Ǿ� �ִٸ�, �浹�� �˻����� ���� �ʴ´�.
	if (!IsActive)
		return;

	// �� ��ü�� �����ִ�. 
	if (IsCollision(leftCol, rightCol))
	{
		if (iter->second)
		{
			// �� �� �ϳ��� ���� �����̰ų�,
			// �� �� �ϳ��� ��Ȱ��ȭ �����̶�� EndOverLap 
			if (IsDead || IsSemiDeactive)
			{
				leftCol->EndOverlap(rightCol);
				rightCol->EndOverlap(leftCol);
				iter->second = false;
			}
			else
			{
				// Overlap (Stay)
				leftCol->Overlap(rightCol);
				rightCol->Overlap(leftCol);
			}
		}
		else
		{
			// �� �� �ϳ��� ���� ������ �ƴϰ�, (and)
			// �� �� �ϳ��� ��Ȱ��ȭ ������ �ƴ� ���� �浹 ����
			if (!IsDead && !IsSemiDeactive)
			{
				// Begin Overlap (Enter)
				leftCol->BeginOverlap(rightCol);
				rightCol->BeginOverlap(leftCol);
				iter->second = true;
			}
		}
	}
	// �� ��ü�� �������� �ʴ�.
	else
	{	
		if (iter->second)
		{
			// �������� �浹 �� �̾���. End Overlap (Exit)
			leftCol->EndOverlap(rightCol);
			rightCol->EndOverlap(leftCol);
		}

		iter->second = false;
	}
}

bool CCollisionManager::IsCollision(CCollider2D* leftCol, CCollider2D* rightCol)
{
	// OBB : �簢�� ���� �浹
	// �� �浹ü�κ��� ���� ��ų ���� ���Ѵ�. (���� ����)
	// 4���� ������ ������ ���� ���Ѽ�
	// �ϳ��� �������� �ʴ� ������ ������ �װ� �������� �ʴ°Ŵ�.
	
	// 0 -- 1
	// |    |
	//   -- 2
	static Vec3 vLocal[4] =
	{
		Vec3(-0.5, 0.5, 0.f),
		Vec3(0.5, 0.5, 0.f),
		Vec3(0.5, -0.5, 0.f),
		Vec3(-0.5, -0.5, 0.f),
	};

	Vec3 vLeftCol[3] = {};
	Vec3 vRightCol[3] = {};

	for (int i = 0; i < 3; i++)
	{
		// XMVector3TransformCoord - Normal ����
		// Coord = ���Ϳ� ����� �� ���� �� ���� ��ǥ(w)�� 1 ����
		// Normal = ���Ϳ� ����� �� ���� �� ���� ��ǥ(w)�� 0 ����
		vLeftCol[i] = XMVector3TransformCoord(vLocal[i], leftCol->GetWorldMat());
		vRightCol[i] = XMVector3TransformCoord(vLocal[i], rightCol->GetWorldMat());
	}

	// ���� ���� ���ϱ� 1�������� 2������ ���� 1���� ���� ������ ���� (���� ��)
	// ���ÿ� ������ �� �� �̱⵵ �ϴ�.
	Vec3 arrProj[4] =
	{
		vLeftCol[1] - vLeftCol[0],
		vLeftCol[2] - vLeftCol[1],
		vRightCol[1] - vRightCol[0],
		vRightCol[2] - vRightCol[1],
	};

	Vec3 vLeftCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), leftCol->GetWorldMat());
	Vec3 vRightCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), rightCol->GetWorldMat());
	Vec3 vCenter = vRightCenter - vLeftCenter;

	// �� ���� ������ 4���� �����۾��� ����
	for (int i = 0; i < 4; i++)
	{
		// ���� ���ͷ� �ٷ� ���������� �ȵ�. ������ ���̸� �ս��ع���.
		Vec3 vProj = arrProj[i];
		// ���� �� (���� ����)
		vProj.Normalize();

		float fProjLen = 0.f;
		for (int j = 0; j < 4; j++)
		{
			// �� ������ ����
			// arrProj[j] --> vProj ���� ����
			fProjLen += fabs(arrProj[j].Dot(vProj));
			// ���� ���͸� ���̸� 1�� ���� ���� ������ �����ϸ� �ٸ��� ������ ���̰� ���´�.
			// �� ���͸� ���� ���ͷ� ���� ���� ������ �����ϸ� �� ���� ���� cos��Ÿ�� ������ ������ ���´�.
			// �ٷ� �غ��� ���̰� ���´�,
		}

		// fProjLen : ���� ���� 4���� ���� ������ ������ ������ ���� ����
		fProjLen *= 0.5f;
		float fCenter = fabs(vCenter.Dot(vProj));

		// �� �� ���̶� fProjLen�� ũ�ٸ� ��ġ�� �ʴ´�.
		if (fProjLen < fCenter)
			return false;
	}

	return true;
}

void CCollisionManager::SaveToLevelFile(FILE* file)
{
	fwrite(m_Matrix, sizeof(UINT), MAX_LAYER, file);
}

void CCollisionManager::LoadToLevelFile(FILE* file)
{
	fread(m_Matrix, sizeof(UINT), MAX_LAYER, file);
}
