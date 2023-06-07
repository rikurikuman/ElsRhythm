#include "SimpleInstance.hlsli"

struct VSInput
{
    float4 pos : POSITION0;
    float4 apos : POSITION1;
    float4 color : COLOR;
    uint instanceID : SV_InstanceID;
};

VSOutput main(VSInput input)
{
	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l

    output.svpos = float4((input.pos + input.apos).xyz, 1.0f);
    output.svpos = mul(matViewProjection, output.svpos); //���W�ɍs�����Z
    output.color = input.color;
	return output;
}