// �}�e���A��
cbuffer ConstBufferDataMaterial : register(b0)
{
    float4 m_color : packoffset(c0); // �F(RGBA)
    float3 m_ambient : packoffset(c1);
    float3 m_diffuse : packoffset(c2);
    float3 m_specular : packoffset(c3);
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float2 uv : TEXCOORD; // uv�l
};