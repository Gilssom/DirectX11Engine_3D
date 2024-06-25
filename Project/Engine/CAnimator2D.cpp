#include "pch.h"
#include "CAnimator2D.h"

#include "CAnim2D.h"

CAnimator2D::CAnimator2D()
	: CComponent(COMPONENT_TYPE::ANIMATOR2D)
	, m_CurAnim(nullptr)
	, m_Repeat(0.f)
{

}

CAnimator2D::CAnimator2D(const CAnimator2D& other)
	: CComponent(other)
	, m_CurAnim(nullptr)
	, m_Repeat(other.m_Repeat)
{
	for (const auto& pair : other.m_mapAnim)
	{
		CAnim2D* pCloneAnim = pair.second->Clone();

		pCloneAnim->m_Animator = this;
		m_mapAnim.insert(make_pair(pair.first, pCloneAnim));
	}

	if (other.m_CurAnim != nullptr)
	{
		m_CurAnim = FindAnimation(other.m_CurAnim->GetName());
	}
}

CAnimator2D::~CAnimator2D()
{
	Safe_Del_Map(m_mapAnim);
}

void CAnimator2D::FinalTick()
{
	if (m_CurAnim == nullptr)
		return;

	if (m_Repeat && m_CurAnim->IsFinish())
	{
		m_CurAnim->Reset();
	}

	m_CurAnim->FinalTick();
}

void CAnimator2D::Binding()
{
	if(m_CurAnim != nullptr)
		m_CurAnim->Binding();
}

void CAnimator2D::CreateAnimation(const wstring& animName, Ptr<CTexture> atlasTex
								, Vec2 leftTopPixelPos, Vec2 vSlicePixelSize, Vec2 backGroundSize, int frameCount, UINT fps)
{
	// ������ Anim �� ��ϵǾ� �ִٸ�
	assert(!FindAnimation(animName));

	CAnim2D* pAnim = new CAnim2D;
	pAnim->SetName(animName);
	pAnim->Create(atlasTex, leftTopPixelPos, vSlicePixelSize, backGroundSize, frameCount, fps);

	// Animation �� ������ ���
	pAnim->m_Animator = this;
	m_mapAnim.insert(make_pair(animName, pAnim));
}

CAnim2D* CAnimator2D::FindAnimation(const wstring& animName)
{
	map<wstring, CAnim2D*>::iterator iter = m_mapAnim.find(animName);

	if (iter == m_mapAnim.end())
		return nullptr;

	return iter->second;
}

CAnim2D* CAnimator2D::FindAnimFile(const wstring& strRelativePath)
{
	CAnim2D* pAnim = new CAnim2D;
	pAnim->Load(strRelativePath);

	return pAnim;
}

void CAnimator2D::LoadAnimation(const wstring& strRelativePath)
{
	CAnim2D* pAnim = new CAnim2D;
	pAnim->Load(strRelativePath);

	auto it = m_mapAnim.find(pAnim->GetName());

	if (it == m_mapAnim.end())
		m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));
	else
		delete pAnim;
}

void CAnimator2D::Play(const wstring& strAnimName, bool repeat)
{
	if(m_CurAnim != nullptr)
		m_CurAnim->Reset();

	m_CurAnim = FindAnimation(strAnimName);
	m_Repeat = repeat;
}

void CAnimator2D::SaveToLevelFile(FILE* file)
{
	// �ִϸ��̼� ���� ����
	size_t animCount = m_mapAnim.size();
	fwrite(&animCount, sizeof(size_t), 1, file);

	// ����ϰ� �ִ� �ִϸ��̼� �̸� ����
	wstring strCurAnimName;
	if (m_CurAnim)
	{
		strCurAnimName = m_CurAnim->GetName();
	}
	SaveWString(strCurAnimName.c_str(), file);

	// �ݺ� ���� ����
	fwrite(&m_Repeat, sizeof(bool), 1, file);

	// ��� �ִϸ��̼� ������ ����
	for (const auto& pair : m_mapAnim)
	{
		pair.second->SaveToLevelFile(file);
	}
}

void CAnimator2D::LoadFromLevelFile(FILE* file)
{
	// �ִϸ��̼� ���� �ε�
	size_t animCount = 0;
	fread(&animCount, sizeof(size_t), 1, file);

	// ����ϰ� �ִ� �ִϸ��̼� �̸� �ε�
	wstring strCurAnimName;
	LoadWString(strCurAnimName, file);

	// �ݺ� ���� �ε�
	fread(&m_Repeat, sizeof(bool), 1, file);

	// ��� �ִϸ��̼� ������ �ε�
	for (size_t i = 0; i < animCount; i++)
	{
		CAnim2D* pAnim = new CAnim2D;
		pAnim->LoadFromLevelFile(file);

		m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));
		pAnim->m_Animator = this;
	}

	// ����ϰ� �ִ� �ִϸ��̼��� ������ ���
	m_CurAnim = FindAnimation(strCurAnimName);
}