#include "pch.h"
#include "CAnimator3D.h"

#include "CAssetManager.h"
#include "CTimeManager.h"
#include "CStructuredBuffer.h"
#include "components.h"

#include "CAnimation3DShader.h"

CAnimator3D::CAnimator3D()
	: CComponent(COMPONENT_TYPE::ANIMATOR3D)
	, m_pVecBones(nullptr)
	, m_pVecClip(nullptr)
	, m_iCurClip(0)
	, m_dCurTime(0.)
	, m_iFrameCount(30)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_iFrameIdx(0)
	, m_iNextFrameIdx(0)
	, m_fRatio(0.f)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& other)
	: CComponent(COMPONENT_TYPE::ANIMATOR3D)
	,m_pVecBones(other.m_pVecBones)
	, m_pVecClip(other.m_pVecClip)
	, m_iCurClip(other.m_iCurClip)
	, m_dCurTime(other.m_dCurTime)
	, m_iFrameCount(other.m_iFrameCount)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_iFrameIdx(other.m_iFrameIdx)
	, m_iNextFrameIdx(other.m_iNextFrameIdx)
	, m_fRatio(other.m_fRatio)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::~CAnimator3D()
{
	if (nullptr != m_pBoneFinalMatBuffer)
		delete m_pBoneFinalMatBuffer;
}

void CAnimator3D::FinalTick()
{
	if (!m_EditorMode || (m_EditorMode && !m_Pause))
	{
		m_dCurTime = 0.f;

		// 현재 재생중인 Clip 의 시간을 진행한다.
		m_vecClipUpdateTime[m_iCurClip] += DT_Engine;

		if (m_iFrameIdx >= m_EndFrame || m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at(m_iCurClip).dTimeLength)
		{
			if (m_IsRepeat)
				m_vecClipUpdateTime[m_iCurClip] = 0.f;
			else
				StopAnimation();
		}

		m_dCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];

		// 현재 프레임 인덱스 구하기
		double dFrameIdx = m_dCurTime * (double)m_iFrameCount + m_StartFrame;
		m_iFrameIdx = (int)(dFrameIdx);

		// Animation Event Trigger
		TriggerEvent(m_iFrameIdx);

		// 다음 프레임 인덱스
		if (m_iFrameIdx >= m_EndFrame)
		{
			m_iNextFrameIdx = m_iFrameIdx;
		}
		else
		{
			m_iNextFrameIdx = m_iFrameIdx + 1;
		}

		// 프레임간의 시간에 따른 비율을 구해준다.
		m_fRatio = (float)(dFrameIdx - (double)m_iFrameIdx);
	}

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* vecAnimClip)
{
	// Test
	//PlayAnimation(0);

	m_pVecClip = vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());

	// 테스트 코드
	/*static float fTime = 0.f;
	fTime += 1.f;
	m_vecClipUpdateTime[0] = fTime;*/
}

void CAnimator3D::SetAnimClip(string name, int startFrame, int endFrame, bool isRepeat)
{
	m_AnimationClip.push_back(AnimationClip(name, startFrame, endFrame, isRepeat));
}

void CAnimator3D::Binding()
{
	if (!m_bFinalMatUpdate)
	{
		// Animation3D Update Compute Shader
		CAnimation3DShader* pUpdateShader = (CAnimation3DShader*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"Animation3DUpdateCS").Get();

		// Bone Data
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneInverseBuffer());
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);

		UINT iBoneCount = (UINT)m_pVecBones->size();
		pUpdateShader->SetBoneCount(iBoneCount);
		pUpdateShader->SetFrameIndex(m_iFrameIdx);
		pUpdateShader->SetNextFrameIdx(m_iNextFrameIdx);
		pUpdateShader->SetFrameRatio(m_fRatio);

		// 업데이트 쉐이더 실행
		pUpdateShader->Execute();

		m_bFinalMatUpdate = true;
	}

	// t17 레지스터에 최종행렬 데이터(구조버퍼) 바인딩		
	m_pBoneFinalMatBuffer->Binding(17);
}

void CAnimator3D::PlayAnimation(int animIndex)
{
	// Test
	m_Pause = false;
	m_StartFrame = m_AnimationClip[animIndex].startFrame;
	m_EndFrame = m_AnimationClip[animIndex].endFrame;
	m_IsRepeat = m_AnimationClip[animIndex].repeat;

	m_vecClipUpdateTime[m_iCurClip] = 0.f;
	m_iFrameIdx = m_StartFrame;
}

void CAnimator3D::StopAnimation()
{
	m_Pause = true;
}

void CAnimator3D::TriggerEvent(int curFrame)
{
	for (auto& event : m_AnimEvents)
	{
		if (event.frame == curFrame)
		{
			if (event.callback)
			{
				// Animation Event 함수 호출
				event.callback();
			}
		}
	}
}

void CAnimator3D::ClearData()
{
	m_pBoneFinalMatBuffer->Clear();

	UINT iMtrlCount = MeshRender()->GetMaterialCount();
	Ptr<CMaterial> pMtrl = nullptr;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		pMtrl = MeshRender()->GetSharedMaterial(i);
		if (nullptr == pMtrl)
			continue;

		pMtrl->SetAnim3D(false); // Animation Mesh 알리기
		pMtrl->SetBoneCount(0);
	}
}

void CAnimator3D::check_mesh(Ptr<CMesh> pMesh)
{
	UINT iBoneCount = pMesh->GetBoneCount();
	if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
	{
		m_pBoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::SRV_UAV, false, nullptr);
	}
}

void CAnimator3D::SaveToLevelFile(FILE* file)
{

}

void CAnimator3D::LoadFromLevelFile(FILE* file)
{

}