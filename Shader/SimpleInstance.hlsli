// �r���[&�ˉe�ϊ��s��
cbuffer ConstBufferDataViewProjection : register(b0)
{
	matrix matViewProjection;
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // ���_���W
    float4 color : COLOR;
};