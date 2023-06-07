#include "Sprite.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
	
	matrix matFinal = mul(matViewProjection, matWorld);
	output.svpos = mul(matFinal, pos); //���W�ɍs�����Z
	output.normal = mul(matWorld, float4(normal, 1.0f)).xyz;
	output.uv = uv;
	return output;
}