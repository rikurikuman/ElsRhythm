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
#include <array>

class Sprite
{
public:
	enum class BlendMode : int32_t {
		Alpha,
		Add,
		Sub
	};

	Material mMaterial;
	Transform mTransform;
	BlendMode mBlendMode = BlendMode::Alpha;

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

	//���_��M��
	void UpdateVertex();

public:
	Sprite();
	Sprite(TextureHandle texture, Vector2 anchor = { 0.5f, 0.5f });

	//�e�N�X�`����ύX����
	void SetTexture(TextureHandle texture);

	//�A���J�[�|�C���g��ύX����
	void SetAnchor(Vector2 anchor);

	/// <summary>
	/// �e�N�X�`���̐؂�o���ʒu��ݒ肷��
	/// </summary>
	/// <param name="srcX">�؂�o���ʒu�̍���X</param>
	/// <param name="srcY">�؂�o���ʒu�̍���Y</param>
	/// <param name="width">�؂�o���͈̓T�C�YX</param>
	/// <param name="height">�؂�o���͈̓T�C�YY</param>
	void SetTexRect(int32_t srcX, int32_t srcY, int32_t width, int32_t height);

	void Init();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer();

	//�`��v����Renderer�֐ς�
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

	GraphicsPipeline GetGraphicsPipeline(int32_t num = 0) {
		return mPipelineStates[num];
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
	std::array<GraphicsPipeline, 3> mPipelineStates;
};

