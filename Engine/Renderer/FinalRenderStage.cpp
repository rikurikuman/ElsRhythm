#include "FinalRenderStage.h"
#include "RDirectX.h"
#include "RenderTarget.h"
#include "RWindow.h"

void FinalRenderStage::Init()
{
	//���_�f�[�^
	VertexPNU vertices[] = {
		{{ -1.0f, -1.0f, 0.0f}, {}, {0.0f, 1.0f}}, //����
		{{ -1.0f, +1.0f, 0.0f}, {}, {0.0f, 0.0f}}, //����
		{{ +1.0f, -1.0f, 0.0f}, {}, {1.0f, 1.0f}}, //�E��
		{{ +1.0f, +1.0f, 0.0f}, {}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));

	rootSignature = RDirectX::GetDefRootSignature();

	// �f�X�N���v�^�����W�̐ݒ�
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�̐ݒ�
	RootParamaters rootParams(1);
	//�e�N�X�`�����W�X�^0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����

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

	pipelineState.desc.VS = Shader("./Shader/FinalVS.hlsl", "main", "vs_5_0");
	pipelineState.desc.PS = Shader("./Shader/FinalPS.hlsl", "main", "ps_5_0");

	// ���X�^���C�U�̐ݒ�
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;
	pipelineState.desc.BlendState.AlphaToCoverageEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = false;
	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();
}

void FinalRenderStage::Render()
{
	D3D12_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(RWindow::GetWidth());
	viewport.Height = static_cast<float>(RWindow::GetHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + RWindow::GetWidth();
	scissorRect.top = 0;
	scissorRect.bottom = scissorRect.top + RWindow::GetHeight();

	RDirectX::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderTarget::SetToBackBuffer();
	RDirectX::GetCommandList()->RSSetViewports(1, &viewport);
	RDirectX::GetCommandList()->RSSetScissorRects(1, &scissorRect);

	RDirectX::GetCommandList()->SetGraphicsRootSignature(rootSignature.ptr.Get());
	RDirectX::GetCommandList()->SetPipelineState(pipelineState.ptr.Get());
	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &vertBuff.view);
	RDirectX::GetCommandList()->IASetIndexBuffer(&indexBuff.view);

	RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureManager::Get(RenderTarget::GetRenderTargetTexture("RenderingImage")->texHandle).gpuHandle);
	RDirectX::GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
