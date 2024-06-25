#pragma once

class CGameObject;
void SpawnObject(int layerIdx, CGameObject* object);

class CLevel;
void ChangeLevelRegister(CLevel* nextLevel, LEVEL_STATE nextLevelState);
void ChangeLevelState(LEVEL_STATE curLevelState);

class CAsset;
void DeleteAsset(Ptr<CAsset> asset);

bool IsValid(CGameObject*& object);

void DrawDebugRect(Vec3 worldPos, Vec3 worldScale, Vec3 worldRotation, Vec4 vColor, float duration);
void DrawDebugRect(Matrix matWorld, Vec4 vColor, float duration);

void DrawDebugCircle(Vec3 worldPos, float radius, Vec4 vColor, float duration);

void DrawDebugLine();

string ToString(const wstring& str);
wstring ToWString(const string& str);

void SaveWString(const wstring& str, FILE* file);
void LoadWString(wstring& str, FILE* file);

int GetRandomInt(int min, int max);
float GetRandomFloat(float min, float max);

void PlaySoundEffect(const wstring& soundPath, float volume = 0.3f);

template<typename T>
class Ptr;

template<typename T>
inline void SaveAssetRef(_In_ Ptr<T> asset, FILE* file)
{
	bool bUse = asset.Get();
	fwrite(&bUse, sizeof(bool), 1, file);

	if (bUse)
	{
		SaveWString(asset->GetKey(), file);
		SaveWString(asset->GetRelativePath(), file);
	}
}

#include "CAssetManager.h"

template<typename T>
inline void LoadAssetRef(_Out_ Ptr<T>& asset, FILE* file)
{
	bool bUse = 0;
	fread(&bUse, sizeof(bool), 1, file);

	if (bUse)
	{
		wstring strKey, strRelativePath;

		LoadWString(strKey, file);
		LoadWString(strRelativePath, file);

		asset = CAssetManager::GetInst()->Load<T>(strKey, strRelativePath);
	}
}


template<typename T, int size>
void Safe_Del_Array(T* (&Array)[size])
{
	for (int i = 0; i < size; ++i)
	{
		if (Array[i] != nullptr)
		{
			delete Array[i];
			Array[i] = nullptr;
		}
	}
}

template<typename T>
void Safe_Del_Vector(vector<T*> vector)
{
	for (size_t i = 0; i < vector.size(); ++i)
	{
		delete vector[i];
		vector[i] = nullptr;
	}

	vector.clear();
}

template<typename T1, typename T2>
void Safe_Del_Map(map<T1, T2>& map)
{
	for (const auto& pair : map)
	{
		if (pair.second != nullptr)
			delete pair.second;
	}

	map.clear();
}

template <typename T>
T Custom_Clamp(T value, T min, T max) 
{
	if (min > max) 
		std::swap(min, max);

	if (value < min) 
		return min;

	if (value > max) 
		return max;

	return value;
}