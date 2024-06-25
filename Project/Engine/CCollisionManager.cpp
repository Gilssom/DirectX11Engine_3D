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
	// 현재 Level 이 설정되어 있지 않다면 return
	if (!CLevelManager::GetInst()->GetCurrentLevel())
		return;

	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Col = Row; Col < MAX_LAYER; ++Col)
		{
			// 해당 칸 비트 체크 여부 판단
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

	// 행이 작은 값, 열이 큰 값으로
	if (Col < Row)
	{
		Row = layerRightIndex;
		Col = layerLeftIndex;
	}

	// 비트 연산자
	UINT CheckBit = (1 << Col);

	// 비트 더하기
	m_Matrix[Row] |= CheckBit;

	//// 만약 해당 비트에 값이 있다면 ( Check Box On , Off 기능 )
	//if (m_Matrix[Row] & CheckBit)
	//{
	//	// 비트 빼기
	//	m_Matrix[Row] &= ~CheckBit;
	//}
	//else
	//{
	//	// 비트 더하기
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

	// 서로 다른 레이어간의 충돌 검사
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
	// 동일한 레이어 내에서 충돌 검사
	else
	{
		for (size_t i = 0; i < vecLeft.size(); i++)
		{
			if (vecLeft[i]->Collider2D() == nullptr)
				continue;

			// 동일한 벡터이므로 동일한 번호끼리나 이미 했던 번호끼리의 검사는 필요 없음
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

	// 두 ID 를 가진 서로의 객체가 충돌한 적이 없다.
	if (iter == m_ColInfo.end())
	{
		m_ColInfo.insert(make_pair(id.ID, false));
		iter = m_ColInfo.find(id.ID);
	}

	// 두 객체 중 하나라도 삭제 예정일 경우
	bool IsDead = leftCol->GetOwner()->IsDead() || rightCol->GetOwner()->IsDead();
	
	// 두 객체 중 하나라도 비활성화일 경우
	bool IsActive = leftCol->IsActive() && rightCol->IsActive();

	// 두 객체 중 하나라도 비활성화 예정일 경우
	bool IsSemiDeactive = leftCol->IsSemiDeactive() || rightCol->IsSemiDeactive();

	// 두 객체 중 하나라도 비활성화가 되어 있다면, 충돌을 검사조차 하지 않는다.
	if (!IsActive)
		return;

	// 두 객체가 겹쳐있다. 
	if (IsCollision(leftCol, rightCol))
	{
		if (iter->second)
		{
			// 둘 중 하나라도 삭제 예정이거나,
			// 둘 중 하나라도 비활성화 예정이라면 EndOverLap 
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
			// 둘 중 하나라도 삭제 예정이 아니고, (and)
			// 둘 중 하나라도 비활성화 예정이 아닐 때만 충돌 진행
			if (!IsDead && !IsSemiDeactive)
			{
				// Begin Overlap (Enter)
				leftCol->BeginOverlap(rightCol);
				rightCol->BeginOverlap(leftCol);
				iter->second = true;
			}
		}
	}
	// 두 객체가 겹쳐있지 않다.
	else
	{	
		if (iter->second)
		{
			// 이전에는 충돌 중 이었다. End Overlap (Exit)
			leftCol->EndOverlap(rightCol);
			rightCol->EndOverlap(leftCol);
		}

		iter->second = false;
	}
}

bool CCollisionManager::IsCollision(CCollider2D* leftCol, CCollider2D* rightCol)
{
	// OBB : 사각형 물리 충돌
	// 각 충돌체로부터 투영 시킬 축을 구한다. (방향 벡터)
	// 4개의 축으로 투영을 각각 시켜서
	// 하나라도 겹쳐있지 않는 각도가 나오면 그건 겹쳐있지 않는거다.
	
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
		// XMVector3TransformCoord - Normal 차이
		// Coord = 벡터와 행렬의 곱 연산 후 동차 좌표(w)에 1 적용
		// Normal = 벡터와 행렬의 곱 연산 후 동차 좌표(w)에 0 적용
		vLeftCol[i] = XMVector3TransformCoord(vLocal[i], leftCol->GetWorldMat());
		vRightCol[i] = XMVector3TransformCoord(vLocal[i], rightCol->GetWorldMat());
	}

	// 방향 벡터 구하기 1정점에서 2정점을 빼면 1번을 향한 방향이 나옴 (투영 축)
	// 동시에 투영이 될 곳 이기도 하다.
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

	// 각 투영 축으로 4번의 투영작업을 진행
	for (int i = 0; i < 4; i++)
	{
		// 단위 벡터로 바로 만들어버리면 안됨. 투영할 길이를 손실해버림.
		Vec3 vProj = arrProj[i];
		// 투영 축 (단위 벡터)
		vProj.Normalize();

		float fProjLen = 0.f;
		for (int j = 0; j < 4; j++)
		{
			// 두 벡터의 내적
			// arrProj[j] --> vProj 으로 투영
			fProjLen += fabs(arrProj[j].Dot(vProj));
			// 한쪽 벡터만 길이를 1로 만든 다음 내적을 진행하면 다른쪽 벡터의 길이가 나온다.
			// 두 벡터를 단위 벡터로 만든 다음 내적을 진행하면 두 벡터 사이 cos세타가 나오니 각도가 나온다.
			// 바로 밑변의 길이가 나온다,
		}

		// fProjLen : 투영 벡터 4개를 투영 축으로 투영한 길이의 합의 절반
		fProjLen *= 0.5f;
		float fCenter = fabs(vCenter.Dot(vProj));

		// 단 한 곳이라도 fProjLen이 크다면 겹치지 않는다.
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
