#pragma once
#include "Obj3D.h"
#include "SRConstBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "Vector2.h"
#include "SRVertexBuffer.h"
#include "SRIndexBuffer.h"

class Image3D final : public Obj3D
{
private:
	TextureHandle mTexture;
	Vector2 mImageScale = { 1, 1 };
	Vector2 mSize = { 1, 1 };
	Vector2 mSrcPos = { 0, 0 };
	Vector2 mTexRect = { 0, 0 };
	Vector2 mAnchor = { 0.5f, 0.5f };
	bool mForceSize = false;
	bool mChangeFlag = false;

	void UpdateVertex();

public:
	enum class BlendMode : int32_t {
		OpaqueAlpha,
		OpaqueAdd,
		OpaqueSub,
		TransparentAlpha,
		TransparentAdd,
		TransparentSub
	};

	BlendMode mBlendMode = BlendMode::OpaqueAlpha;

private:
	GraphicsPipeline GetPipeline();

public:
	Material mMaterial;
	//Transform��Obj3D�ɂ���

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<MaterialBuffer> mMaterialBuff;
	SRConstBuffer<TransformBuffer> mTransformBuff;
	SRConstBuffer<ViewProjectionBuffer> mViewProjectionBuff;

	Image3D();

	Image3D(TextureHandle texture, Vector2 size = {1, 1}, bool forceSize = false);

	void SetTexture(TextureHandle texture);

	TextureHandle GetTexture() {
		return mTexture;
	}

	void SetSize(Vector2 size, bool forceSize = false);

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

	Vector2 GetSize() {
		return mSize;
	}

	//����������
	void Init();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��v����Renderer��
	void Draw() override;
};

