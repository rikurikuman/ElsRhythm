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
	void SetTexRect(int srcX, int srcY, int width, int height);

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

