#pragma once
#include <d3d12.h>
#include "Vector2.h"
#include "Texture.h"
#include "Material.h"
#include "Transform.h"
#include "RConstBuffer.h"
#include "ViewProjection.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class PostEffect
{
public:
	Material material;

public:
	TextureHandle texture;

	VertexBuffer vertBuff;
	IndexBuffer indexBuff;
	RConstBuffer<MaterialBuffer> materialBuff;

public:
	PostEffect();
	PostEffect(TextureHandle texture, Vector2 anchor = { 0.5f, 0.5f });

	//テクスチャを変更する
	void SetTexture(TextureHandle texture);

	void Init();

	//各データのバッファへの転送
	void TransferBuffer();

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands();

	static void InitPipeline();

public:
	static RootSignature rootSignature;
	static GraphicsPipeline pipelineState;
};