#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
	
	// �@�����񂷂��߂ɕ��s�ړ��������������s���p�ӂ���
    matrix worldRot = matWorld;
    worldRot[0][3] = 0;
    worldRot[1][3] = 0;
    worldRot[2][3] = 0;
	
	matrix matFinal = mul(matViewProjection, matWorld);
    output.wpos = mul(matWorld, pos).xyz;
	output.svpos = mul(matFinal, pos); // ���W�ɍs�����Z
    output.normal = normalize(mul(worldRot, float4(normal, 1.0f)).xyz);
	output.uv = uv;
	return output;
}