#include "pch.h"
#include "CAnimator3D.h"

#include "CAssetManager.h"
#include "CTimeManager.h"
#include "CLevelManager.h"
#include "CLevel.h"
#include "CStructuredBuffer.h"
#include "components.h"

#include "CAnimation3DShader.h"
#include "CScript.h"

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
	, m_pVecBones(other.m_pVecBones)
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

	if (!m_AnimationClip.empty())
		m_AnimationClip.clear();

	if (!m_AnimEvents.empty())
		m_AnimEvents.clear();
}

void CAnimator3D::FinalTick()
{
	if (CLevelManager::GetInst()->GetCurrentLevel()->GetState() == LEVEL_STATE::STOP)
		return;

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
	m_CurPlayIndex = animIndex;
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

void CAnimator3D::DeleteEvent(int frame)
{
	// remove if 의 조건에 맞는 요소 탐색 및 제거

	m_AnimEvents.erase
	(
		std::remove_if(m_AnimEvents.begin(), m_AnimEvents.end(),
			[frame](const AnimationEvent& event)
			{
				return event.frame == frame;
			}),
		m_AnimEvents.end()
	);
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

void CAnimator3D::Save(const wstring& relativePath)
{
	wstring strFilePath = CPathManager::GetInst()->GetContentPath() + relativePath + GetName() + L".anim";

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	if (pFile == nullptr)
	{
		MessageBox(nullptr, L"애니메이션 저장 실패", L"애니메이션 저장 오류", MB_OK);
		return;
	}

	// 애니메이션 이름 저장
	SaveWString(GetName(), pFile);

	// 애니메이션 클립 개수 저장
	size_t clipCount = m_AnimationClip.size();
	fwrite(&clipCount, sizeof(size_t), 1, pFile);

	// 각 애니메이션 클립 저장
	for (size_t i = 0; i < clipCount; ++i)
	{
		const AnimationClip& clip = m_AnimationClip[i];

		// 클립 이름 저장
		SaveWString(ToWString(clip.animName), pFile);

		// 시작 및 종료 프레임 저장
		fwrite(&clip.startFrame, sizeof(int), 1, pFile);
		fwrite(&clip.endFrame, sizeof(int), 1, pFile);

		// 반복 여부 저장
		fwrite(&clip.repeat, sizeof(bool), 1, pFile);

		// 애니메이션 이벤트 개수 저장
		size_t eventCount = clip.haveEvent.size();
		fwrite(&eventCount, sizeof(size_t), 1, pFile);

		// 각 이벤트 저장
		for (const auto& event : clip.haveEvent)
		{
			// 이벤트가 발생하는 프레임 번호 저장
			fwrite(&event.frame, sizeof(int), 1, pFile);
		
			// 이벤트 이름 저장
			SaveWString(ToWString(event.eventName), pFile);
		}

		// 연결된 애니메이션 정보 저장
		//SaveWString(clip.linkedAnimation, pFile);
	}

	fclose(pFile);
}

void CAnimator3D::Load(const wstring& relativePath)
{
	wstring strFilePath = CPathManager::GetInst()->GetContentPath() + relativePath + GetName();

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	if (pFile == nullptr)
	{
		MessageBox(nullptr, L"애니메이션 불러오기 실패", L"애니메이션 불러오기 오류", MB_OK);
		return;
	}

	// 애니메이션 이름 로드
	wstring animName;
	LoadWString(animName, pFile);

	// 애니메이션 클립 개수 로드
	size_t clipCount = 0;
	fread(&clipCount, sizeof(size_t), 1, pFile);

	// 기존 애니메이션 클립을 비우고 새 데이터를 불러옴
	m_AnimationClip.clear();
	m_AnimationClip.reserve(clipCount);

	// 각 애니메이션 클립 로드
	for (size_t i = 0; i < clipCount; ++i)
	{
		AnimationClip clip;

		// 클립 이름 로드
		wstring wClipName;
		LoadWString(wClipName, pFile);
		clip.animName = ToString(wClipName);

		// 시작 및 종료 프레임 로드
		fread(&clip.startFrame, sizeof(int), 1, pFile);
		fread(&clip.endFrame, sizeof(int), 1, pFile);

		// 반복 여부 로드
		fread(&clip.repeat, sizeof(bool), 1, pFile);

		// 애니메이션 이벤트 개수 로드
		size_t eventCount = 0;
		fread(&eventCount, sizeof(size_t), 1, pFile);

		// 각 이벤트 로드
		for (size_t j = 0; j < eventCount; ++j)
		{
			AnimationEvent event;

			// 이벤트가 발생하는 프레임 번호 로드
			fread(&event.frame, sizeof(int), 1, pFile);

			// 이벤트 이름 로드
			wstring wEventName;
			LoadWString(wEventName, pFile);
			event.eventName = ToString(wEventName);

			// 이벤트 콜백은 직렬화하지 않으므로 추후 이벤트를 추가하는 방식으로 처리
			const vector<CScript*>& pVecScripts = GetOwner()->GetScripts();
			for (size_t i = 0; i < pVecScripts.size(); i++)
			{
				event.callback = pVecScripts[i]->GetEventCallbackByName(event.eventName);

				if (event.callback != nullptr)
					break;
			}

			AddEvent(event.frame, event.callback, event.eventName);
			clip.haveEvent.push_back(AnimationClip::SaveEvent({ event.frame, event.eventName }));
		}

		m_AnimationClip.push_back(clip);
	}

	fclose(pFile);
}

void CAnimator3D::SaveToLevelFile(FILE* file)
{

}

void CAnimator3D::LoadFromLevelFile(FILE* file)
{

}