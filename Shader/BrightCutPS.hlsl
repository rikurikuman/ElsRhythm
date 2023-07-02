#include "BrightCut.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(OutputVS i) : SV_TARGET
{
    float4 color = tex.Sample(smp, i.uv);
    float4 t = dot(color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    float4 extract = smoothstep(0.6, 0.9, t);
    return color * extract;
}