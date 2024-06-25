#ifndef _TILEMAP
#define _TILEMAP

#include "value.fx"

struct tTileInfo
{
    uint ImageIndex;
    float3 vPadding;
};

struct VS_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// =================================
//  Tile Map Shader
//  Parameter
#define AtlasMaxRow         g_int_0
#define AtlasMaxCol         g_int_1
#define TileImageIndex      g_int_2
#define AtlasResolution     g_vec2_0
#define AtlasTileSize       g_vec2_1
#define TileColRow          g_vec2_2

#define IsCaptureMode       g_int_3

StructuredBuffer<tTileInfo> g_TileInfo : register(t20);
// ==================================

VS_OUT VS_TileMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if (IsCaptureMode)
    {
        output.vPosition = float4(float3(_in.vLocalPos * 2.f), 1.f);
    }
    else
    {
        _in.vLocalPos.xy += float2(0.5f, -0.5f);
        output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    }  
    
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_TileMap(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    // 1. �Էµ� UV (0 ~ 1 ����) �� ���� Ÿ�� ��� ������ ���߾� Ȯ���Ѵ�.
    _in.vUV *= TileColRow;
    
    // 2. Ȯ���Ų UV ���� �ش� �ȼ� Shader �� Ÿ�� �󿡼��� ��ġ���� ��ķ� �˾Ƴ� �� �ִ�. ex) (2.8, 2.8) => 2�� 2��
    int2 PixelColRowIndex = (int2)_in.vUV;
    
    // 3. Ȯ���Ų UV ���� �Ҽ����� ���Ѵ�. �ش� �ȼ��� �ش� ĭ ������ ��� ��ġ���� �� �� �ִ�.
    _in.vUV = _in.vUV - floor(_in.vUV);
    
    // 4. Atlas Texture �󿡼� �ϳ��� Ÿ�� �̹����� �����ϴ� ũ�⸦ UV ������ ���Ѵ�.
    float2 TileSizeUV = AtlasTileSize / AtlasResolution;
    
    // 5. �ش� �ȼ��� �����ؾ� �ϴ� Ÿ�� �»�ܿ��� ������ UV ��ǥ�� ���Ѵ�.
    float2 vUV = TileSizeUV * _in.vUV;
    
    // 6. �ȼ��� ��ġ�ϰ� �ִ� Ÿ���� ������ �˾Ƴ���, �� Ÿ���� Image Index ������ ����ȭ���ۿ��� ������.
    int ImgIndex = g_TileInfo[PixelColRowIndex.y * TileColRow.x + PixelColRowIndex.x].ImageIndex;
    
    // 7. Atlas �󿡼� ���� �����Ϸ��� Ÿ���� �»�� UV �� ���Ѵ�.
    int row = ImgIndex / AtlasMaxCol;
    int col = ImgIndex % AtlasMaxCol;
    float2 LeftTop = float2(col * TileSizeUV.x, row * TileSizeUV.y);
    
    // 8. Atlas ��ü ������ �ش� �̹��� �»�� ��ǥ + �̹��� �»�ܿ��� ������ ��ġ UV
    vUV = LeftTop + vUV;
    
    // 9. ���� ���ø�
    vOutColor = g_tex_0.Sample(g_sam_0, vUV);
    
    return vOutColor;
}

#endif