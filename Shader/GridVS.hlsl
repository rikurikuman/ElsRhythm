#include "Grid.hlsli"

VSOutput main(float4 pos : POSITION)
{
	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l

	output.svpos = mul(matViewProjection, pos); //���W�ɍs�����Z
	return output;
}