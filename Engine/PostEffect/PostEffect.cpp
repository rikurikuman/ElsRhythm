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
	//頂点データ
	VertexPNU vertices[] = {
		{{ -0.5f, -0.5f, 0.0f}, {}, {0.0f, 1.0f}}, //左下
		{{ -0.5f, +0.5f, 0.0f}, {}, {0.0f, 0.0f}}, //左上
		{{ +0.5f, -0.5f, 0.0f}, {}, {1.0f, 1.0f}}, //右下
		{{ +0.5f, +0.5f, 0.0f}, {}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
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
	//頂点バッファビューの設定コマンド
	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &vertBuff.view);

	//インデックスバッファビューの設定コマンド
	RDirectX::GetCommandList()->IASetIndexBuffer(&indexBuff.view);

	//定数バッファビューの設定コマンド
	RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(1, materialBuff.constBuff->GetGPUVirtualAddress());

	//SRVヒープから必要なテクスチャデータをセットする
	RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureManager::Get(texture).gpuHandle);

	TransferBuffer();

	//描画コマンド
	RDirectX::GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0); // 全ての頂点を使って描画
}

void PostEffect::InitPipeline()
{
	rootSignature = RDirectX::GetDefRootSignature();

	// デスクリプタレンジの設定
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメータの設定
	RootParamaters rootParams(5);
	//テクスチャレジスタ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ0番(Material)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	rootSignature.desc.RootParamaters = rootParams;

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //リニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f; //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダーからだけ見える
	rootSignature.desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	rootSignature.Create();

	pipelineState = RDirectX::GetDefPipeline();

	pipelineState.desc.VS = Shader("./Shader/PostEffectVS.hlsl", "main", "vs_5_0");
	pipelineState.desc.PS = Shader("./Shader/PostEffectPS.hlsl", "main", "ps_5_0");

	// ラスタライザの設定
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;
	pipelineState.desc.BlendState.AlphaToCoverageEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = false;
	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();
}