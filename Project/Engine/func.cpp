#include "pch.h"

#include "CDbgRenderManager.h"
#include "CTaskManager.h"

#include "CGameObject.h"

#include <random>

std::mt19937 randomValue(std::random_device{}());

// ������Ʈ ���� �Լ�
void SpawnObject(int layerIdx, CGameObject* object)
{
	tTask task = {};
	task.Type = TASK_TYPE::SPAWN_OBJECT;
	task.dwParam_0 = layerIdx;
	task.dwParam_1 = (DWORD_PTR)object;

	CTaskManager::GetInst()->AddTask(task);
}

// Level ���� �Լ�
void ChangeLevelRegister(CLevel* nextLevel, LEVEL_STATE nextLevelState)
{
	tTask task = {};
	task.Type = TASK_TYPE::CHANGE_LEVEL;
	task.dwParam_0 = (DWORD_PTR)nextLevel;
	task.dwParam_1 = (DWORD_PTR)nextLevelState;

	CTaskManager::GetInst()->AddTask(task);
}

// Level ���� ���� �Լ�
void ChangeLevelState(LEVEL_STATE curLevelState)
{
	tTask task = {};
	task.Type = TASK_TYPE::CHANGE_LEVEL_STATE;
	task.dwParam_0 = (DWORD_PTR)curLevelState;

	CTaskManager::GetInst()->AddTask(task);
}

// Asset ���� �Լ�
void DeleteAsset(Ptr<CAsset> asset)
{
	tTask task = {};
	task.Type = TASK_TYPE::DELETE_ASSET;
	task.dwParam_0 = (DWORD_PTR)asset.Get();

	CTaskManager::GetInst()->AddTask(task);
}

// ������Ʈ ��ȿ�� üũ
bool IsValid(CGameObject*& object)
{
	// Ȥ�� �ش� ������Ʈ �ּҰ� null ����
	if (object == nullptr)
	{
		return false;
	}
	// �ƴϸ� �ش� ������Ʈ�� ���� ��������
	else if (object->IsDead())
	{
		object = nullptr;
		return false;
	}
	// �װ͵� �ƴ϶�� �ش� ������Ʈ�� ���� �����Ѵ�.
	else
	{
		return true;
	}
}

// �� ������ ���� �Լ�
void DrawDebugRect(Vec3 worldPos, Vec3 worldScale, Vec3 worldRotation, Vec4 vColor, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;

	info.Position = worldPos;
	info.Scale = worldScale;
	info.Rotation = worldRotation;
	info.matWorld = XMMatrixIdentity();

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

// ��� ���� �Լ�
void DrawDebugRect(Matrix matWorld, Vec4 vColor, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;
	info.matWorld = matWorld;

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(Vec3 worldPos, float radius, Vec4 vColor, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CIRCLE;

	info.Position = worldPos;
	info.Scale = Vec3(radius * 2.f, radius * 2.f, 1.f);
	info.matWorld = XMMatrixIdentity();

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugSphere(Vec3 worldPos, float radius, Vec4 vColor, bool depthTest, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::SPHERE;

	info.Position = worldPos;
	info.Scale = Vec3(radius * 2.f, radius * 2.f, radius * 2.f);
	info.matWorld = XMMatrixIdentity();

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

string ToString(const wstring& str)
{
	return string(str.begin(), str.end());
}

wstring ToWString(const string& str)
{
	return wstring(str.begin(), str.end());
}

void SaveWString(const wstring& str, FILE* file)
{
	// �߿�** wstring ��ü�� ���������� ���ڿ��� �����ϴ� ���� �ƴ϶� �ּҸ� �����ϴ� ��ü�̴�.
	// 1. �����ͷ� ����Ų Key ���� �ּ�, 
	// 2. w_char �� ũ�⸸ŭ, 
	// 3. ���ڿ��� ���̸�ŭ ����

	size_t len = str.length();
	fwrite(&len, sizeof(size_t), 1, file);
	fwrite(str.c_str(), sizeof(wchar_t), len, file);
}

void LoadWString(wstring& str, FILE* file)
{
	//wchar_t szBuffer[256] = {};

	size_t len = 0;
	fread(&len, sizeof(size_t), 1, file);

	str.resize(len);

	fread((wchar_t*)str.c_str(), sizeof(wchar_t), len, file);
}

int GetRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);

	return dist(randomValue);
}

float GetRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);

	return dist(randomValue);
}

void PlaySoundEffect(const wstring& soundPath, float volume)
{
	Ptr<CSound> pNewBgm = CAssetManager::GetInst()->FindAsset<CSound>(soundPath);
	pNewBgm->Play(1, volume, true);
}