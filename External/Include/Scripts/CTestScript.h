#pragma once
#include <Engine\\CScript.h>

class CTestScript :
    public CScript
{
public:
    virtual void Tick() override;

public:
    CLONE(CTestScript);

public:
    CTestScript();
    ~CTestScript();
};

