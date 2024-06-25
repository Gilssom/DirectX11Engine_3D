#pragma once
#include "CRenderComponent.h"

struct tTileInfo
{
    UINT    ImageIndex; // 본인이 몇번 Image 를 참조할 것인지
    Vec3    vPadding;
};

class CStructuredBuffer;

class CTileMap : public CRenderComponent
{
private:
    UINT                m_Row;               // Tile Row Count
    UINT                m_Col;               // Tile Colum Count
    Vec2                m_TileEachSize;      // 각 타일 1개의 크기 (인게임 내의 크기)

    Ptr<CTexture>       m_Atlas;             // Use Atlas Texture
    UINT                m_AtlasMaxRow;       // Atlas Max Row (최대 행 크기)
    UINT                m_AtlasMaxCol;       // Atlas Max Col (최대 열 크기)
    Vec2                m_AtlasTileEachSize; // Atlas Texture 각 타일 1개의 크기
    Vec2                m_AtlasResolution;   // Atlas Texture 해상도 정보

    vector<tTileInfo>   m_vecTileInfo;
    CStructuredBuffer*  m_TileBuffer;        // 깊은 복사의 대상 (구조화 버퍼)

    Ptr<CTexture>       m_CaptureTex;
    Vec2                m_CaptureTexMaxResolution;
    bool                m_EditorCapture;     // Editor 전용 Image 전달 여부

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

