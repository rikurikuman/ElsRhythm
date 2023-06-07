#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	texcolor = texcolor * m_color;
	
	//����x
    const float shininess = 4.0f;
	
	//���_�ւ̃x�N�g��
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//�����ˌ�
    float3 ambient = m_ambient;
	
	//�V�F�[�f�B���O���ʂ̐F
    float4 shadecolor = float4(ambientColor * ambient, 1);
	
    //���s����
    for (int i = 0; i < DIRECTIONAL_LIGHT_NUM; i++)
    {
        if (directionalLights[i].active)
        {
            //�����ւ̃x�N�g���Ɩ@���̓���
            float dotNormal = dot(-directionalLights[i].lightVec, input.normal);
		    //���ˌ��x�N�g��
            float3 reflect = normalize(directionalLights[i].lightVec + 2.0f * dotNormal * input.normal);
		    //�g�U���ˌ�
            float3 diffuse = saturate(dotNormal) * m_diffuse;
		    //���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
		
            shadecolor.rgb += (diffuse + specular) * directionalLights[i].lightColor;
        }
    }
    
    //�_����
    for (i = 0; i < POINT_LIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
            float3 lightVec = pointLights[i].pos - input.wpos;
            float d = length(lightVec);
            lightVec = normalize(lightVec);
            
            //���������W��
            float atten = 1.0f / (pointLights[i].atten.x + pointLights[i].atten.y * d + pointLights[i].atten.z * d * d);
            
            float dotNormal = dot(lightVec, input.normal);
            //���ˌ��x�N�g��
            float3 reflect = normalize(-lightVec + 2.0f * dotNormal * input.normal);
            //�g�U���ˌ�
            float3 diffuse = saturate(dotNormal) * m_diffuse;
            //���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
            
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].color;
        }
    }

    //�X�|�b�g���C�g
    for (i = 0; i < SPOT_LIGHT_NUM; i++)
    {
        if(spotLights[i].active)
        {
            float3 lightVec = spotLights[i].pos - input.wpos;
            float d = length(lightVec);
            lightVec = normalize(lightVec);
            
            //���������W��
            float atten = saturate(1.0f / (spotLights[i].atten.x + spotLights[i].atten.y * d + spotLights[i].atten.z * d * d));
            
            //�p�x����
            float cos = dot(lightVec, -spotLights[i].dir);
            
            //�����J�n�p�x����A�����I���p�x�ɂ����Č���
            float angleatten = smoothstep(spotLights[i].factorAngleCos.y, spotLights[i].factorAngleCos.x, cos);
            
            //�p�x��������Z
            atten *= angleatten;
            
            float dotNormal = dot(lightVec, input.normal);
            //���ˌ��x�N�g��
            float3 reflect = normalize(-lightVec + 2.0f * dotNormal * input.normal);
            //�g�U���ˌ�
            float3 diffuse = saturate(dotNormal) * m_diffuse;
            //���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
            
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].color;
        }
    }
    
    return texcolor * shadecolor;
}