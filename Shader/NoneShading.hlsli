// マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
    float4 m_color : packoffset(c0); // 色(RGBA)
    float3 m_ambient : packoffset(c1);
    float3 m_diffuse : packoffset(c2);
    float3 m_specular : packoffset(c3);
};

// ワールド変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
    matrix matWorld;
};

// ビュー&射影変換行列
cbuffer ConstBufferDataViewProjection : register(b2)
{
    matrix matViewProjection : packoffset(c0);
    float3 cameraPos : packoffset(c4);
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
    float3 wpos : WORLD_POSITION; //ワールド頂点座標
    float4 svpos : SV_POSITION; // システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; // uv値
};