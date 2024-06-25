#pragma once

class CLevelSaveLoad
{
public:
	static void SaveLevel(CLevel* level, const wstring& filePath);
	static void SaveGameObject(CGameObject* object, FILE* file);

	static CLevel* LoadLevel(const wstring& filePath);
	static CGameObject* LoadGameObject(FILE* file);
};