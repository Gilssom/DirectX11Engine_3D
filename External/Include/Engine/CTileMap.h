#pragma once
#include "CRenderComponent.h"

struct tTileInfo
{
    UINT    ImageIndex; // ������ ��� Image �� ������ ������
    Vec3    vPadding;
};

class CStructuredBuffer;

class CTileMap : public CRenderComponent
{
private:
    UINT                m_Row;               // Tile Row Count
    UINT                m_Col;               // Tile Colum Count
    Vec2                m_TileEachSize;      // �� Ÿ�� 1���� ũ�� (�ΰ��� ���� ũ��)

    Ptr<CTexture>       m_Atlas;             // Use Atlas Texture
    UINT                m_AtlasMaxRow;       // Atlas Max Row (�ִ� �� ũ��)
    UINT                m_AtlasMaxCol;       // Atlas Max Col (�ִ� �� ũ��)
    Vec2                m_AtlasTileEachSize; // Atlas Texture �� Ÿ�� 1���� ũ��
    Vec2                m_AtlasResolution;   // Atlas Texture �ػ� ����

    vector<tTileInfo>   m_vecTileInfo;
    CStructuredBuffer*  m_TileBuffer;        // ���� ������ ��� (����ȭ ����)

    Ptr<CTexture>       m_CaptureTex;
    Vec2                m_CaptureTexMaxResolution;
    bool                m_EditorCapture;     // Editor ���� Image ���� ����

public:
    void SetRowCol(UINT row, UINT col);
    void SetAtlasTexture(Ptr<CTexture> tex);
    void SetAtlasTileSize(Vec2 tileSize);
    void SetTileEachSize(Vec2 size);
    void SetCapture(bool capture) { m_EditorCapture = capture; }
    void SetImageIndex(int row, int col, int imageIndex);

    UINT GetRow() { return m_Row; }
    UINT GetCol() { return m_Col; }
    Vec2 GetTileSize() { return m_TileEachSize; }

    Ptr<CTexture> GetAtlas() { return m_Atlas; }
    UINT GetAtlasMaxRow() { return m_AtlasMaxRow; }
    UINT GetAtlasMaxCol() { return m_AtlasMaxCol; }
    Vec2 GetAtlasTileSize() { return m_AtlasTileEachSize; }
    Ptr<CTexture> GetCaptureTexture() { return m_CaptureTex; }

public:
    virtual void Begin() override;
    virtual void FinalTick() override;
    virtual void Render() override;

private:
    void CaptureRender();
    void CheckCaptureTexture();

public:
    virtual void SaveToLevelFile(FILE* file) override;
    virtual void LoadFromLevelFile(FILE* file) override;

    CLONE(CTileMap);

public:
    CTileMap();
    CTileMap(const CTileMap& other);
    ~CTileMap();
};

