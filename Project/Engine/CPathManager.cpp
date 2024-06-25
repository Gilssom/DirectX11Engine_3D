#include "pch.h"
#include "CPathManager.h"

CPathManager::CPathManager()
	: m_szContentPath{}
{

}

CPathManager::~CPathManager()
{

}

void CPathManager::Init()
{
	GetCurrentDirectory(255, m_szContentPath);

	size_t len = wcslen(m_szContentPath);

	for (size_t i = len - 1; i > 0; i--)
	{
		if (m_szContentPath[i] == '\\')
		{
			m_szContentPath[i] = '\0';
			break;
		}
	}

	wcscat_s(m_szContentPath, L"\\content\\");
}

const wstring CPathManager::GetRelativePath(const wstring& absolutePath)
{
	// m_szContentPath 길이 이후 부터 구하면 상대경로 추출이 가능
	size_t len = wcslen(m_szContentPath);
	wstring strRelativePath = absolutePath.substr(len, absolutePath.length() - len);

	return strRelativePath;
}

const wstring CPathManager::GetRelativeName(const wstring& absolutePath, const wstring& folderName)
{
	// m_szContentPath 길이에 소속 폴더 이름 이후 부터 구하면 파일 이름 추출이 가능
	size_t len = wcslen(m_szContentPath) + folderName.length();
	wstring strRelativeName = absolutePath.substr(len, absolutePath.length() - len);

	return strRelativeName;
}
