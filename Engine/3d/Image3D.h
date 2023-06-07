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
	TextureHandle texture;
	Vector2 scale = { 1, 1 };
	Vector2 size = { 1, 1 };
	bool forceSize = false;

public:
	Material material;
	//Transform��Obj3D�ɂ���

	SRVertexBuffer vertBuff;
	SRIndexBuffer indexBuff;
	SRConstBuffer<MaterialBuffer> materialBuff;
	SRConstBuffer<TransformBuffer> transformBuff;
	SRConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	Image3D() {};

	Image3D(TextureHandle texture, Vector2 size = {1, 1}, bool forceSize = false);

	void SetTexture(TextureHandle tex) {
		texture = tex;
		if (!forceSize) {
			Texture tex = TextureManager::Get(texture);
			this->size.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * size.x;
			this->size.y = size.y;
		}
	}

	TextureHandle GetTexture() {
		return texture;
	}

	void SetSize(Vector2 size, bool forceSize = false) {
		this->forceSize = forceSize;
		if (forceSize) {
			this->size = size;
		}
		else {
			scale = size;
			Texture tex = TextureManager::Get(texture);
			this->size.x = tex.resource->GetDesc().Width / (float)tex.resource->GetDesc().Height * scale.x;
			this->size.y = scale.y;
		}
	}

	Vector2 GetSize() {
		return size;
	}

	//����������
	void Init();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��v����Renderer��
	void Draw() override;
};

