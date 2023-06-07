#include "SimpleMesh.hlsli"

struct VSInput
{
    float4 pos : POSITION0;
    float4 color : COLOR;
};

VSOutput main(VSInput input)
{
	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l

    output.svpos = float4(input.pos.xy, 1.0f, 1.0f);
    output.color = input.color;
	return output;
}