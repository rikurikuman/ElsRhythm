#include "PostEffect.h"
#include <d3d12.h>
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"

using namespace Microsoft::WRL;

RootSignature PostEffect::rootSignature;
GraphicsPipeline PostEffect::pipelineState;

PostEffect::PostEffect()
{
	Init();
}

PostEffect::PostEffect(TextureHandle texture, Vector2 anchor)
{
	this->texture = texture;
	Init();
}

void PostEffect::SetTexture(TextureHandle texture)
{
	this->texture = texture;
}

void PostEffect::Init()
{
	//���_�f�[�^
	VertexPNU vertices[] = {
		{{ -0.5f, -0.5f, 0.0f}, {}, {0.0f, 1.0f}}, //����
		{{ -0.5f, +0.5f, 0.0f}, {}, {0.0f, 0.0f}}, //����
		{{ +0.5f, -0.5f, 0.0f}, {}, {1.0f, 1.0f}}, //�E��
		{{ +0.5f, +0.5f, 0.0f}, {}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));
}

void PostEffect::TransferBuffer()
{
	material.Transfer(materialBuff.constMap);
}

void PostEffect::DrawCommands()
{
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &vertBuff.view);

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetCommandList()->IASetIndexBuffer(&indexBuff.view);

	//�萔�o�b�t�@�r���[�̐ݒ�R�}���h
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());

	//SRV�q�[�v����K�v�ȃe�N�X�`���f�[�^���Z�b�g����
	RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureManager::Get(texture).gpuHandle);

	TransferBuffer();

	//�`��R�}���h
	RDirectX::GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void PostEffect::InitPipeline()
{
	rootSignature = RDirectX::GetDefRootSignature();

	// �f�X�N���v�^�����W�̐ݒ�
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�̐ݒ�
	RootParamaters rootParams(5);
	//�e�N�X�`�����W�X�^0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@0��(Material)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[1].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParams[1].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����

	rootSignature.desc.RootParamaters = rootParams;

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //���j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f; //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //�s�N�Z���V�F�[�_�[���炾��������
	rootSignature.desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	rootSignature.Create();

	pipelineState = RDirectX::GetDefPipeline();

	pipelineState.desc.VS = Shader("./Shader/PostEffectVS.hlsl", "main", "vs_5_0");
	pipelineState.desc.PS = Shader("./Shader/PostEffectPS.hlsl", "main", "ps_5_0");

	// ���X�^���C�U�̐ݒ�
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;
	pipelineState.desc.BlendState.AlphaToCoverageEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = false;
	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();
}