#pragma once
#include "EditorUI.h"

class MenuUI : public EditorUI
{
private:

public:
    void File();

    void GameObject();

    void Level();

    void Asset();

    void RenderTarget();

private:
    wstring GetAssetDefaultName(wstring baseName);
    void RenderTargetFilter(const string& menuItemName, const wstring& targetKey);

public:
    virtual void Tick() override;
    virtual void Render_Tick() override;

public:
    MenuUI();
    ~MenuUI();
};

