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

private:
    wstring GetAssetDefaultName(wstring baseName);

public:
    virtual void Tick() override;
    virtual void Render_Tick() override;

public:
    MenuUI();
    ~MenuUI();
};

