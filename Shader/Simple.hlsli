// �F
cbuffer ConstBufferDataColor : register(b0)
{
	float4 color;
}

// �r���[&�ˉe�ϊ��s��
cbuffer ConstBufferDataViewProjection : register(b1)
{
	matrix matViewProjection;
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
	float4 svpos : SV_POSITION;// �V�X�e���p���_���W
};