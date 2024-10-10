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

		// ���� ������� Clip �� �ð��� �����Ѵ�.
		m_vecClipUpdateTime[m_iCurClip] += DT_Engine;

		if (m_iFrameIdx >= m_EndFrame || m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at(m_iCurClip).dTimeLength)
		{
			if (m_IsRepeat)
				m_vecClipUpdateTime[m_iCurClip] = 0.f;
			else
				StopAnimation();
		}

		m_dCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];

		// ���� ������ �ε��� ���ϱ�
		double dFrameIdx = m_dCurTime * (double)m_iFrameCount + m_StartFrame;
		m_iFrameIdx = (int)(dFrameIdx);

		// Animation Event Trigger
		TriggerEvent(m_iFrameIdx);

		// ���� ������ �ε���
		if (m_iFrameIdx >= m_EndFrame)
		{
			m_iNextFrameIdx = m_iFrameIdx;
		}
		else
		{
			m_iNextFrameIdx = m_iFrameIdx + 1;
		}

		// �����Ӱ��� �ð��� ���� ������ �����ش�.
		m_fRatio = (float)(dFrameIdx - (double)m_iFrameIdx);
	}

	// ��ǻƮ ���̴� ���꿩��
	m_bFinalMatUpdate = false;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* vecAnimClip)
{
	// Test
	//PlayAnimation(0);

	m_pVecClip = vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());

	// �׽�Ʈ �ڵ�
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

		// ������Ʈ ���̴� ����
		pUpdateShader->Execute();

		m_bFinalMatUpdate = true;
	}

	// t17 �������Ϳ� ������� ������(��������) ���ε�		
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
				// Animation Event �Լ� ȣ��
				event.callback();
			}
		}
	}
}

void CAnimator3D::DeleteEvent(int frame)
{
	// remove if �� ���ǿ� �´� ��� Ž�� �� ����

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

		pMtrl->SetAnim3D(false); // Animation Mesh �˸���
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
		MessageBox(nullptr, L"�ִϸ��̼� ���� ����", L"�ִϸ��̼� ���� ����", MB_OK);
		return;
	}

	// �ִϸ��̼� �̸� ����
	SaveWString(GetName(), pFile);

	// �ִϸ��̼� Ŭ�� ���� ����
	size_t clipCount = m_AnimationClip.size();
	fwrite(&clipCount, sizeof(size_t), 1, pFile);

	// �� �ִϸ��̼� Ŭ�� ����
	for (size_t i = 0; i < clipCount; ++i)
	{
		const AnimationClip& clip = m_AnimationClip[i];

		// Ŭ�� �̸� ����
		SaveWString(ToWString(clip.animName), pFile);

		// ���� �� ���� ������ ����
		fwrite(&clip.startFrame, sizeof(int), 1, pFile);
		fwrite(&clip.endFrame, sizeof(int), 1, pFile);

		// �ݺ� ���� ����
		fwrite(&clip.repeat, sizeof(bool), 1, pFile);

		// �ִϸ��̼� �̺�Ʈ ���� ����
		size_t eventCount = clip.haveEvent.size();
		fwrite(&eventCount, sizeof(size_t), 1, pFile);

		// �� �̺�Ʈ ����
		for (const auto& event : clip.haveEvent)
		{
			// �̺�Ʈ�� �߻��ϴ� ������ ��ȣ ����
			fwrite(&event.frame, sizeof(int), 1, pFile);
		
			// �̺�Ʈ �̸� ����
			SaveWString(ToWString(event.eventName), pFile);
		}

		// ����� �ִϸ��̼� ���� ����
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
		MessageBox(nullptr, L"�ִϸ��̼� �ҷ����� ����", L"�ִϸ��̼� �ҷ����� ����", MB_OK);
		return;
	}

	// �ִϸ��̼� �̸� �ε�
	wstring animName;
	LoadWString(animName, pFile);

	// �ִϸ��̼� Ŭ�� ���� �ε�
	size_t clipCount = 0;
	fread(&clipCount, sizeof(size_t), 1, pFile);

	// ���� �ִϸ��̼� Ŭ���� ���� �� �����͸� �ҷ���
	m_AnimationClip.clear();
	m_AnimationClip.reserve(clipCount);

	// �� �ִϸ��̼� Ŭ�� �ε�
	for (size_t i = 0; i < clipCount; ++i)
	{
		AnimationClip clip;

		// Ŭ�� �̸� �ε�
		wstring wClipName;
		LoadWString(wClipName, pFile);
		clip.animName = ToString(wClipName);

		// ���� �� ���� ������ �ε�
		fread(&clip.startFrame, sizeof(int), 1, pFile);
		fread(&clip.endFrame, sizeof(int), 1, pFile);

		// �ݺ� ���� �ε�
		fread(&clip.repeat, sizeof(bool), 1, pFile);

		// �ִϸ��̼� �̺�Ʈ ���� �ε�
		size_t eventCount = 0;
		fread(&eventCount, sizeof(size_t), 1, pFile);

		// �� �̺�Ʈ �ε�
		for (size_t j = 0; j < eventCount; ++j)
		{
			AnimationEvent event;

			// �̺�Ʈ�� �߻��ϴ� ������ ��ȣ �ε�
			fread(&event.frame, sizeof(int), 1, pFile);

			// �̺�Ʈ �̸� �ε�
			wstring wEventName;
			LoadWString(wEventName, pFile);
			event.eventName = ToString(wEventName);

			// �̺�Ʈ �ݹ��� ����ȭ���� �����Ƿ� ���� �̺�Ʈ�� �߰��ϴ� ������� ó��
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