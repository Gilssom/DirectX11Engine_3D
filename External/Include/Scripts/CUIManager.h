#pragma once

#include <Engine\Singletone.h>
#include "UIElement.h"
#include "UIDamage.h"

struct UIPosition
{
	string name;
	float x;
	float y;
};

class CPlayerScript;

class CUIManager : public CSingleton<CUIManager>
{
	SINGLE(CUIManager);

private:
	vector<std::shared_ptr<UIElement>>	m_UIElement;
	vector<std::shared_ptr<UIDamage>>	m_UIDamage;
	vector<UIPosition>					m_UIPositions;
	CPlayerScript*						m_PlayerScript;

public:
	void SetPlayer(CPlayerScript* player) { m_PlayerScript = player; }
	CPlayerScript* GetPlayer() { return m_PlayerScript; }

	void ChangeLevel(const wstring& levelName);

public:
	void AddUIElement(std::shared_ptr<UIElement>& element) { m_UIElement.push_back(element); }
	void AddUIDamage(int damage, const ImVec2& position) { m_UIDamage.push_back(std::make_shared<UIDamage>(damage, position, ImGui::GetTime())); }

	template<typename T>
	std::shared_ptr<T> FindUIElementByType();

public:
	void Init();
	void Tick();

public:
	void SaveUIPositions(const string& filename);
	void LoadUIPositions(const string& filename);
	void UpdateUIPositions();
};

template<typename T>
inline std::shared_ptr<T> CUIManager::FindUIElementByType()
{
	for (const auto& element : m_UIElement)
	{
		std::shared_ptr<T> targetElement = std::dynamic_pointer_cast<T>(element);

		if (targetElement)
			return targetElement;
	}

	return nullptr;
}