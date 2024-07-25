#pragma once
#include "CShader.h"

#pragma region GPGPU - Compute Shader Info
// General Purpose GPU = GPGPU = ���� �� ��� Shader = Compute Shader _ DX11
// NVIDIA CUDA : C++ �� ����� ������ �������� GPU �� ������ �� ���� ( NVIDIA GPU �� ��� ���� )
// ��� �׷���ī��� NVIDIA - AMD �� ���� ������ ����ϱ� ������
// ���뼺�� ����� �ְ� ������ �ؾ��Ѵ�. ( = Compute Shader )

// CPU �� GPU �� ���Ұ� �۾��� ������ ���� ����
// CPU : ���� �ڻ� 2~3�� (����)
// GPU : �뵿�� 1~10���� (ȿ��) - �ܼ� �۾� (Rendering) - GPU Thread_���� ó��

// Compute Shader �� ��𿡼� ����ϳ�
// - Particle System - �������� ����
// ������Ʈ �ϳ��� ��û�� ������ ���� ���ڸ� ������ �� �ְ� �ȴ�.
// GPU ���� ������ ���� �� �����ӿ��� �ѹ��� Tick �� ���� �� �ְ� �Ǳ� ����.
#pragma endregion

// ���� �۾� GPU
class CComputeShader : public CShader
{
private:
    ComPtr<ID3DBlob>                m_CSBlob;
    ComPtr<ID3D11ComputeShader>     m_CS;


protected:
    tMtrlConst                      m_Const;


protected:
    // 1���� �׷� �� ������ ����
    UINT                            m_ThreadPerGroupX;
    UINT                            m_ThreadPerGroupY;
    UINT                            m_ThreadPerGroupZ;

    // �׷� ����
    UINT                            m_GroupX;
    UINT                            m_GroupY;
    UINT                            m_GroupZ;


public:
    virtual int Binding() = 0;
    virtual void CalculateGroupNum() = 0;
    virtual void Clear() = 0;


public:
    int Execute();


private:
    int CreateComputeShader(const wstring& strFilePath, const string& psFuncName);


public:
    CComputeShader(UINT threadPerGroupX, UINT threadPerGroupY, UINT threadPerGroupZ
                , const wstring& relativePath, const string& funcName);
    ~CComputeShader();
};

