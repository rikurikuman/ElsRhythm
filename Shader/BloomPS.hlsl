#include "Bloom.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(OutputVS i) : SV_TARGET
{
    return tex.Sample(smp, i.uv);
}