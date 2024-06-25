#include "pch.h"
#include "CAnim2D.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CTexture.h"
#include "CTimeManager.h"
#include "CPathManager.h"
#include "CAssetManager.h"

CAnim2D::CAnim2D()
	: m_Animator(nullptr)
	, m_CurIdx(0)
	, m_Finish(false)
	, m_Time(0.f)
{

}

CAnim2D::CAnim2D(const CAnim2D& other)
	: CEntity(other)
	, m_Animator(nullptr)
	, m_vecFrame(other.m_vecFrame)
	, m_BackGroundSize(other.m_BackGroundSize)
	, m_CurIdx(other.m_CurIdx)
	, m_Finish(other.m_Finish)
	, m_Time(other.m_Time)
	, m_AtlasTex(other.m_AtlasTex)
{

}

CAnim2D::~CAnim2D()
{
	
}

void CAnim2D::Create(Ptr<CTexture> atlasTex, Vec2 leftTopPixelPos, Vec2 vSlicePixelSize, Vec2 backGroundSize, int frameCount, UINT fps)
{
	assert(atlasTex.Get());

	m_AtlasTex = atlasTex;

	m_BackGroundSize.x = backGroundSize.x / m_AtlasTex->GetWidth();
	m_BackGroundSize.y = backGroundSize.y / m_AtlasTex->GetHeight();

	for (int i = 0; i < frameCount; i++)
	{
		tAnim2DFrame frame = {};

		frame.LeftTop.x = (leftTopPixelPos.x + vSlicePixelSize.x * i) / m_AtlasTex->GetWidth();
		frame.LeftTop.y = leftTopPixelPos.y / m_AtlasTex->GetHeight();

		frame.Duration = 1.f / (float)fps;

		frame.SliceSize.x = vSlicePixelSize.x / m_AtlasTex->GetWidth();
		frame.SliceSize.y = vSlicePixelSize.y / m_AtlasTex->GetHeight();

		m_vecFrame.push_back(frame);
	}
}

void CAnim2D::Create(Ptr<CTexture> atlasTex, vector<tAnim2DFrame>& animFrames, int frameCount)
{
	assert(atlasTex.Get());

	m_AtlasTex = atlasTex;

	m_BackGroundSize.x = 150.f / m_AtlasTex->GetWidth();
	m_BackGroundSize.y = 150.f / m_AtlasTex->GetHeight();

	for (int i = 0; i < frameCount; i++)
	{
		tAnim2DFrame frame = {};

		frame.LeftTop.x = animFrames[i].LeftTop.x / m_AtlasTex->GetWidth();
		frame.LeftTop.y = animFrames[i].LeftTop.y / m_AtlasTex->GetHeight();

		frame.Duration = 1.f / animFrames[i].Duration;

		frame.SliceSize.x = animFrames[i].SliceSize.x / m_AtlasTex->GetWidth();
		frame.SliceSize.y = animFrames[i].SliceSize.y / m_AtlasTex->GetHeight();

		m_vecFrame.push_back(frame);
	}
}

void CAnim2D::FinalTick()
{
	if (m_Finish)
		return;

	m_Time += DT;

	// �ִϸ��̼� ��� �ð��� �Ѿ�� �������� �Ѱ��ش�.
	if (m_vecFrame[m_CurIdx].Duration < m_Time)
	{
		if(m_CurIdx == 0 && m_StartCallBack)
			m_StartCallBack();

		m_Time = m_Time - m_vecFrame[m_CurIdx].Duration;
		++m_CurIdx;

		if (m_EventCallBack)
			m_EventCallBack();

		// �ִ� �ִϸ��̼��� �Ѿ�� Finish
		if ((int)m_vecFrame.size() <= m_CurIdx)
		{
			m_CurIdx = m_vecFrame.size() - 1;
			m_Finish = true;

			if (m_EndCallBack)
				m_EndCallBack();
		}
	}
}

void CAnim2D::Binding()
{
	// ��Ʋ�� �ؽ���
	if (m_AtlasTex != nullptr)
		m_AtlasTex->Binding(14);

	// ������ ����
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION);

	tAnim2DInfo info = {};
	info.vLeftTop = m_vecFrame[m_CurIdx].LeftTop;
	info.vSliceSize = m_vecFrame[m_CurIdx].SliceSize;
	info.vBackGround = m_BackGroundSize;
	info.UseAnim2D = 1;

	pCB->SetData(&info);
	pCB->Binding();
}

void CAnim2D::Clear()
{
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION);
	pCB->Clear();
}

void CAnim2D::Save(const wstring& relativeFolderPath)
{
	wstring strFilePath = CPathManager::GetInst()->GetContentPath() + relativeFolderPath + GetName() + L".anim";

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	if (pFile == nullptr)
	{
		MessageBox(nullptr, L"�ִϸ��̼� ���� ����", L"�ִϸ��̼� ���� ����", MB_OK);
		return;
	}

	// �ִϸ��̼� �̸� ����
	SaveWString(GetName(), pFile);

	// ������ ���� ����
	size_t i = m_vecFrame.size();
	fwrite(&i, sizeof(size_t), 1, pFile);

	// ������ ���� ����
	// vector �� ���� �迭�̱� ������ ������
	fwrite(m_vecFrame.data(), sizeof(tAnim2DFrame), i, pFile);

	// Back Ground Size ���� ���� 
	fwrite(&m_BackGroundSize, sizeof(Vec2), 1, pFile);

	// ��Ʋ�� �ؽ�ó ����
	SaveAssetRef(m_AtlasTex, pFile);

	fclose(pFile);
}

void CAnim2D::Load(const wstring& relativeFolderPath)
{
	wstring strFilePath = CPathManager::GetInst()->GetContentPath() + relativeFolderPath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	if (pFile == nullptr)
	{
		MessageBox(nullptr, L"�ִϸ��̼� Load ����", L"�ִϸ��̼� Load ����", MB_OK);
		return;
	}

	// �ִϸ��̼� �̸� �ε�
	wstring strName;
	LoadWString(strName, pFile);
	SetName(strName);

	// ������ ���� �ε�
	size_t i = 0;
	fread(&i, sizeof(size_t), 1, pFile);

	// ������ ���� �ε�
	// reserve : Ȯ���� ũ�⸦ �� �� ����ȭ�� ���� (������ �ִµ� ���� ��� �־ ���� X)
	// resize : ũ�⸦ �ø��� ������ Ư���� ������ ä���. (���� ����)
	m_vecFrame.resize(i);
	fread(m_vecFrame.data(), sizeof(tAnim2DFrame), i, pFile);

	// Back Ground Size ���� �ε�
	fread(&m_BackGroundSize, sizeof(Vec2), 1, pFile);

	// ��Ʋ�� �ؽ�ó �ε�
	LoadAssetRef(m_AtlasTex, pFile);

	fclose(pFile);
}

void CAnim2D::SaveToLevelFile(FILE* file)
{
	// �ִϸ��̼��� �̸� ����
	SaveWString(GetName(), file);

	// ������ ������ ���� ���� (vector Container �� Ư¡)
	size_t frameCount = m_vecFrame.size();
	fwrite(&frameCount, sizeof(size_t), 1, file);
	fwrite(m_vecFrame.data(), sizeof(tAnim2DFrame), m_vecFrame.size(), file);

	// BackGround ũ�� ����
	fwrite(&m_BackGroundSize, sizeof(Vec2), 1, file);

	// �����ϴ� Atlas Texture ���� ����
	SaveAssetRef(m_AtlasTex, file);
}

void CAnim2D::LoadFromLevelFile(FILE* file)
{
	// �ִϸ��̼��� �̸� �ε�
	wstring Name;
	LoadWString(Name, file);
	SetName(Name);

	// ������ ������ ���� �ε� (vector Container �� Ư¡)
	size_t frameCount = 0;
	fread(&frameCount, sizeof(size_t), 1, file);
	m_vecFrame.resize(frameCount); // resize �� resurve ���� �ľ�
	fread(m_vecFrame.data(), sizeof(tAnim2DFrame), m_vecFrame.size(), file);

	// BackGround ũ�� �ε�
	fread(&m_BackGroundSize, sizeof(Vec2), 1, file);

	// �����ϴ� Atlas Texture ���� �ε�
	LoadAssetRef(m_AtlasTex, file);
}