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

	// 애니메이션 재생 시간을 넘어서면 다음으로 넘겨준다.
	if (m_vecFrame[m_CurIdx].Duration < m_Time)
	{
		if(m_CurIdx == 0 && m_StartCallBack)
			m_StartCallBack();

		m_Time = m_Time - m_vecFrame[m_CurIdx].Duration;
		++m_CurIdx;

		if (m_EventCallBack)
			m_EventCallBack();

		// 최대 애니메이션을 넘어서면 Finish
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
	// 아틀라스 텍스쳐
	if (m_AtlasTex != nullptr)
		m_AtlasTex->Binding(14);

	// 프레임 정보
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
		MessageBox(nullptr, L"애니메이션 저장 실패", L"애니메이션 저장 오류", MB_OK);
		return;
	}

	// 애니메이션 이름 저장
	SaveWString(GetName(), pFile);

	// 프레임 개수 저장
	size_t i = m_vecFrame.size();
	fwrite(&i, sizeof(size_t), 1, pFile);

	// 프레임 정보 저장
	// vector 가 동적 배열이기 때문에 가능함
	fwrite(m_vecFrame.data(), sizeof(tAnim2DFrame), i, pFile);

	// Back Ground Size 정보 저장 
	fwrite(&m_BackGroundSize, sizeof(Vec2), 1, pFile);

	// 아틀라스 텍스처 저장
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
		MessageBox(nullptr, L"애니메이션 Load 실패", L"애니메이션 Load 오류", MB_OK);
		return;
	}

	// 애니메이션 이름 로드
	wstring strName;
	LoadWString(strName, pFile);
	SetName(strName);

	// 프레임 개수 로드
	size_t i = 0;
	fread(&i, sizeof(size_t), 1, pFile);

	// 프레임 정보 로드
	// reserve : 확정적 크기를 알 때 최적화를 위한 (공간은 있는데 텅텅 비어 있어서 접근 X)
	// resize : 크기를 늘리고 공간을 특정한 값으로 채운다. (접근 가능)
	m_vecFrame.resize(i);
	fread(m_vecFrame.data(), sizeof(tAnim2DFrame), i, pFile);

	// Back Ground Size 정보 로드
	fread(&m_BackGroundSize, sizeof(Vec2), 1, pFile);

	// 아틀라스 텍스처 로드
	LoadAssetRef(m_AtlasTex, pFile);

	fclose(pFile);
}

void CAnim2D::SaveToLevelFile(FILE* file)
{
	// 애니메이션의 이름 저장
	SaveWString(GetName(), file);

	// 프레임 개수와 정보 저장 (vector Container 의 특징)
	size_t frameCount = m_vecFrame.size();
	fwrite(&frameCount, sizeof(size_t), 1, file);
	fwrite(m_vecFrame.data(), sizeof(tAnim2DFrame), m_vecFrame.size(), file);

	// BackGround 크기 저장
	fwrite(&m_BackGroundSize, sizeof(Vec2), 1, file);

	// 참조하던 Atlas Texture 정보 저장
	SaveAssetRef(m_AtlasTex, file);
}

void CAnim2D::LoadFromLevelFile(FILE* file)
{
	// 애니메이션의 이름 로드
	wstring Name;
	LoadWString(Name, file);
	SetName(Name);

	// 프레임 개수와 정보 로드 (vector Container 의 특징)
	size_t frameCount = 0;
	fread(&frameCount, sizeof(size_t), 1, file);
	m_vecFrame.resize(frameCount); // resize 와 resurve 차이 파악
	fread(m_vecFrame.data(), sizeof(tAnim2DFrame), m_vecFrame.size(), file);

	// BackGround 크기 로드
	fread(&m_BackGroundSize, sizeof(Vec2), 1, file);

	// 참조하던 Atlas Texture 정보 로드
	LoadAssetRef(m_AtlasTex, file);
}