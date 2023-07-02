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
	Material mMaterial;
	Transform mTransform;

private:
	bool mChangeFlag = false;
	TextureHandle mTexture;
	Vector2 mSrcPos = { 0, 0 };
	Vector2 mSize = { 0, 0 };
	Vector2 mAnchor = { 0.5f, 0.5f };

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<MaterialBuffer> mMaterialBuff;
	SRConstBuffer<TransformBuffer> mTransformBuff;
	SRConstBuffer<ViewProjectionBuffer> mViewProjectionBuff;

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
	void SetTexRect(int32_t srcX, int32_t srcY, int32_t width, int32_t height);

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
		return mRootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return mPipelineState;
	}

private:
	SpriteManager() {
		Init();
	};
	~SpriteManager() = default;
	SpriteManager(const SpriteManager&) {};
	SpriteManager& operator=(const SpriteManager&) { return *this; }

	void Init();

	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;
};

