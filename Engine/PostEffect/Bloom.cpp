#include "Bloom.h"
#include <RDirectX.h>
#include <Renderer.h>
#include <RenderTarget.h>

Bloom::Bloom()
{
	//���_�f�[�^
	VertexPU vertices[] = {
		{{ -1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, //����
		{{ -1.0f, +1.0f, 0.0f}, {0.0f, 0.0f}}, //����
		{{ +1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, //�E��
		{{ +1.0f, +1.0f, 0.0f}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));

	RenderTarget::CreateRenderTargetTexture(1280, 720, 0x000000, "BloomA");
	RenderTarget::CreateRenderTargetTexture(1280, 720, 0x000000, "BloomB");
}

void Bloom::Draw()
{
	RenderTarget::GetRenderTargetTexture("BloomA")->ClearRenderTarget();
	RenderTarget::GetRenderTargetTexture("BloomA")->ClearDepthStencil();
	RenderTarget::GetRenderTargetTexture("BloomB")->ClearRenderTarget();
	RenderTarget::GetRenderTargetTexture("BloomB")->ClearDepthStencil();

	//RenderOrder orderA;
	//orderA.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	//orderA.vertBuff = vertBuff;
	//orderA.indexBuff = indexBuff;
	//orderA.indexCount = indexBuff.GetIndexCount();
	//orderA.anchorPoint = Vector3(0, 0, 0);
	//orderA.rootSignature = GetRootSignature().ptr.Get();
	//orderA.pipelineState = GetGraphicsPipelineA().ptr.Get();
	//orderA.renderTargets = { "BloomA" };
	//orderA.rootData = {
	//	{ TextureManager::Get(texture).gpuHandle }
	//};
	//Renderer::DrawCall("PostEffect", orderA);

	RenderOrder orderB;
	orderB.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	orderB.vertBuff = vertBuff;
	orderB.indexBuff = indexBuff;
	orderB.indexCount = indexBuff.GetIndexCount();
	orderB.anchorPoint = Vector3(0, 0, 0);
	orderB.rootSignature = GetRootSignature().ptr.Get();
	orderB.pipelineState = GetGraphicsPipelineB().ptr.Get();
	orderB.renderTargets = { "BloomB" };
	orderB.rootData = {
		{ TextureManager::Get(RenderTarget::GetRenderTargetTexture("RenderingImage")->texHandle).gpuHandle}
	};
	Renderer::DrawCall("PostEffect", orderB);

	RenderOrder orderC;
	orderC.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	orderC.vertBuff = vertBuff;
	orderC.indexBuff = indexBuff;
	orderC.indexCount = indexBuff.GetIndexCount();
	orderC.anchorPoint = Vector3(0, 0, 0);
	orderC.rootSignature = GetRootSignature().ptr.Get();
	orderC.pipelineState = GetGraphicsPipelineC().ptr.Get();
	orderC.renderTargets = { "RenderingImage" };
	orderC.rootData = {
		{ TextureManager::Get(RenderTarget::GetRenderTargetTexture("BloomB")->texHandle).gpuHandle}
	};
	Renderer::DrawCall("PostEffect", orderC);
}

RootSignature& Bloom::GetRootSignature()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().desc;

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	RootParamaters rootParams(1);
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	desc.RootParamaters = rootParams;

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
	desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };

	return RootSignature::GetOrCreate("Bloom", desc);
}

GraphicsPipeline& Bloom::GetGraphicsPipelineA()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().desc;

	desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	desc.VS = Shader::GetOrCreate("BloomA_VS", "Shader/BrightCutVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("BloomA_PS", "Shader/BrightCutPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignature().ptr.Get();
	return GraphicsPipeline::GetOrCreate("BloomA", desc);
}

GraphicsPipeline& Bloom::GetGraphicsPipelineB()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().desc;

	desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	desc.VS = Shader::GetOrCreate("BloomB_VS", "Shader/GaussianBlurVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("BloomB_PS", "Shader/GaussianBlurPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignature().ptr.Get();
	return GraphicsPipeline::GetOrCreate("BloomB", desc);
}

GraphicsPipeline& Bloom::GetGraphicsPipelineC()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().desc;

	desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	desc.VS = Shader::GetOrCreate("BloomC_VS", "Shader/BloomVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("BloomC_PS", "Shader/BloomPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignature().ptr.Get();
	return GraphicsPipeline::GetOrCreate("BloomC", desc);
}
