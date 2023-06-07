// �r���[&�ˉe�ϊ��s��
cbuffer ConstBufferDataViewProjection : register(b0)
{
	matrix matViewProjection;
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // ���_���W
    float2 uv : TEXCOORD; // uv�l
};

// �W�I���g���V�F�[�_�[�̏o�͍\����
struct GSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float2 uv : TEXCOORD; // uv�l
};