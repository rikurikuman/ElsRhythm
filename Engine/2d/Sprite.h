#pragma once
#include <d3d12.h>
#include "Vector2.h"
#include "Texture.h"
#include "Material.h"
#include "Transform.h"
#include "SRConstBuffer.h"
#include "ViewProjection.h"
#include "SRVertexBuffer.h"
#include "SRIndexBuffer.h"

class Sprite
{
public:
	Material material;
	Transform transform;

private:
	bool change = false;
	TextureHandle texture;
	Vector2 srcPos = { 0, 0 };
	Vector2 size = { 0, 0 };
	Vector2 anchor = { 0.5f, 0.5f };

	SRVertexBuffer vertBuff;
	SRIndexBuffer indexBuff;
	SRConstBuffer<MaterialBuffer> materialBuff;
	SRConstBuffer<TransformBuffer> transformBuff;
	SRConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	//頂点を弄る
	void UpdateVertex();

public:
	Sprite();
	Sprite(TextureHandle texture, Vector2 anchor = { 0.5f, 0.5f });

	//テクスチャを変更する
	void SetTexture(TextureHandle texture);

	//アンカーポイントを変更する
	void SetAnchor(Vector2 anchor);

	/// <summary>
	/// テクスチャの切り出し位置を設定する
	/// </summary>
	/// <param name="srcX">切り出し位置の左上X</param>
	/// <param name="srcY">切り出し位置の左上Y</param>
	/// <param name="width">切り出し範囲サイズX</param>
	/// <param name="height">切り出し範囲サイズY</param>
	void SetTexRect(int srcX, int srcY, int width, int height);

	void Init();

	//各データのバッファへの転送
	void TransferBuffer();

	//描画要求をRendererへ積む
	void Draw();
};

class SpriteManager
{
public:
	static SpriteManager* GetInstance() {
		static SpriteManager instance;
		return &instance;
	}

	RootSignature GetRootSignature() {
		return rootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return pipelineState;
	}

private:
	SpriteManager() {
		Init();
	};
	~SpriteManager() = default;
	SpriteManager(const SpriteManager& a) {};
	SpriteManager& operator=(const SpriteManager&) { return *this; }

	void Init();

	RootSignature rootSignature;
	GraphicsPipeline pipelineState;
};

