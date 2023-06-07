#include "GaussianBlur.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(OutputVS i) : SV_TARGET
{
    float totalWeight = 0, _Sigma = 0.002, _StepWidth = 0.001;
    float4 col;
    
    [loop]
    for (float py = -_Sigma * 2; py <= _Sigma * 2; py += _StepWidth)
    {
        [loop]
        for (float px = -_Sigma * 2; px <= _Sigma * 2; px += _StepWidth)
        {
            float2 pickUV = i.uv + float2(px, py);
            float weight = Gaussian(i.uv, pickUV, _Sigma);
            col += tex.Sample(smp, pickUV) * weight;
            totalWeight += weight;
        }
    }
    col.rgb = col.rgb / totalWeight;
    col.a = 1;
    return col;
}