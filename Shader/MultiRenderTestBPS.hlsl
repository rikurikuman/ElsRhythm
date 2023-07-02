#include "MultiRenderTest.hlsli"

Texture2D<float4> tex0 : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1 : register(t1); //1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(OutputVS i) : SV_TARGET
{
    float4 color0 = tex0.Sample(smp, i.uv);
    float4 color1 = tex1.Sample(smp, i.uv);
    
    float4 color = color0;
    if (fmod(i.uv.y, 0.1f) < 0.05f)
    {
        color = color1;
    }
    
    return color;
}