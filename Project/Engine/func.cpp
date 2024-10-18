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
void DrawDebugRect(Vec3 worldPos, Vec3 worldScale, Vec3 worldRotation, Vec4 vColor, float depthTest, float duration)
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

	info.DepthTest = depthTest;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

// ��� ���� �Լ�
void DrawDebugRect(Matrix matWorld, Vec4 vColor, float depthTest, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;
	info.matWorld = matWorld;

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	info.DepthTest = depthTest;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(Vec3 worldPos, float radius, Vec4 vColor, float depthTest, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CIRCLE;

	info.Position = worldPos;
	info.Scale = Vec3(radius * 2.f, radius * 2.f, 1.f);
	info.matWorld = XMMatrixIdentity();

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	info.DepthTest = depthTest;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCube(Vec3 worldPos, Vec3 worldScale, Vec3 worldRotation, Vec4 vColor, bool depthTest, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CUBE;

	info.Position = worldPos;
	info.Scale = worldScale;
	info.Rotation = worldRotation;
	info.matWorld = XMMatrixIdentity();

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	info.DepthTest = depthTest;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCube(Matrix matWorld, Vec4 vColor, float depthTest, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CUBE;
	info.matWorld = matWorld;

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	info.DepthTest = depthTest;

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

	info.DepthTest = depthTest;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugSphere(Matrix matWorld, Vec4 vColor, float depthTest, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::SPHERE;
	info.matWorld = matWorld;

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	info.DepthTest = depthTest;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCone(Vec3 worldPos, Vec3 worldScale, Vec3 worldRotation, Vec4 vColor, bool depthTest, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CONE;

	info.Position = worldPos;
	info.Scale = worldScale;
	info.Rotation = worldRotation;
	info.matWorld = XMMatrixIdentity();

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	info.DepthTest = depthTest;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCone(Matrix matWorld, Vec4 vColor, float depthTest, float duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CONE;
	info.matWorld = matWorld;

	info.Color = vColor;

	info.Duration = duration;
	info.Age = 0.f;

	info.DepthTest = depthTest;

	CDbgRenderManager::GetInst()->AddDebugShapeInfo(info);
}

Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat)
{
	Matrix mat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.m[i][j] = (float)_mat.Get(i, j);
		}
	}
	return mat;
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

Vec3 QuaternionToEuler(const Vec4& q)
{
	Vec3 euler;

	// Roll (X�� ȸ��)
	float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	euler.x = atan2(sinr_cosp, cosr_cosp);

	// Pitch (Y�� ȸ��)
	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (abs(sinp) >= 1)
		euler.y = copysign(XM_PI / 2, sinp); // 90�� (����/2) ����
	else
		euler.y = asin(sinp);

	// Yaw (Z�� ȸ��)
	float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	euler.z = atan2(siny_cosp, cosy_cosp);

	return euler;
}
