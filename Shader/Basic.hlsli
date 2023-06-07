// �}�e���A��
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 m_color : packoffset(c0); // �F(RGBA)
	float3 m_ambient : packoffset(c1);
	float3 m_diffuse : packoffset(c2);
	float3 m_specular : packoffset(c3);
};

// ���[���h�ϊ��s��
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix matWorld;
};

// �r���[&�ˉe�ϊ��s��
cbuffer ConstBufferDataViewProjection : register(b2)
{
    matrix matViewProjection : packoffset(c0);
    float3 cameraPos : packoffset(c4);
};

// ���s�����^
struct DirectionalLight
{
    uint active;
    float3 lightVec;
    float3 lightColor;
};

//�_�����^
struct PointLight
{
    uint active;
    float3 pos;
    float3 color;
    float3 atten;
};

//�X�|�b�g���C�g�^
struct SpotLight
{
    uint active;
    float3 pos;
    float3 dir;
    float3 color;
    float3 atten;
    float2 factorAngleCos;
};

// �������
static const int DIRECTIONAL_LIGHT_NUM = 8;
static const int POINT_LIGHT_NUM = 8;
static const int SPOT_LIGHT_NUM = 8;
cbuffer ConstBufferDataLight : register(b3)
{
    float3 ambientColor;
    DirectionalLight directionalLights[DIRECTIONAL_LIGHT_NUM];
    PointLight pointLights[POINT_LIGHT_NUM];
    SpotLight spotLights[SPOT_LIGHT_NUM];
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
    float3 wpos : WORLD_POSITION; //���[���h���_���W
	float4 svpos : SV_POSITION;// �V�X�e���p���_���W
	float3 normal : NORMAL; //�@���x�N�g��
	float2 uv : TEXCOORD;// uv�l
};