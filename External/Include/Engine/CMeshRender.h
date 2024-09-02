#pragma once
#include "CRenderComponent.h"

class CMeshRender : public CRenderComponent
{
public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void Render(UINT iSubset) override;

    CLONE(CMeshRender);

public:
    CMeshRender();
    ~CMeshRender();
};

