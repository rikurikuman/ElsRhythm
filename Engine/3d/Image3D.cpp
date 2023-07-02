#include "Image3D.h"
#include "RDirectX.h"
#include "Vertex.h"
#include "LightGroup.h"
#include <Renderer.h>

Image3D::Image3D(TextureHandle texture, Vector2 size, bool forceSize)
{
	mTexture = texture;

	if (forceSize) {
		mSize.x = size.x;
		mSize.y = size.y;
	}
	else {
		mScale = size;
		Texture tex = TextureManager::Get(texture);
		mSize.x = tex.mResource->GetDesc().Width / (float)tex.mResource->GetDesc().Height * mScale.x;
		mSize.y = mScale.y;
	}

	Init();
}
	
void Image3D::Init()
{
	//���_�f�[�^
	VertexPNU vertices[] = {
		{{ -0.5f * mSize.x, -0.5f * mSize.y, 0.0f }, {}, {0.0f, 1.0f}}, //����
		{{ -0.5f * mSize.x,  0.5f * mSize.y, 0.0f }, {}, {0.0f, 0.0f}}, //����
		{{  0.5f * mSize.x, -0.5f * mSize.y, 0.0f }, {}, {1.0f, 1.0f}}, //�E��
		{{  0.5f * mSize.x,  0.5f * mSize.y, 0.0f }, {}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	uint32_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	VertexPNU::CalcNormalVec(vertices, indices, _countof(indices));

	mVertBuff.Init(vertices, _countof(vertices));
	mIndexBuff.Init(indices, _countof(indices));
}

void Image3D::TransferBuffer(ViewProjection viewprojection)
{
	mMaterial.Transfer(mMaterialBuff.Get());
	mTransform.Transfer(mTransformBuff.Get());
	mViewProjectionBuff->matrix = viewprojection.mMatrix;
}

void Image3D::Draw()
{
	std::vector<RootData> rootData{
		{TextureManager::Get(mTexture).mGpuHandle},
		{RootDataType::SRBUFFER_CBV, mMaterialBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff },
		{RootDataType::LIGHT},
	};

	Renderer::DrawCall("Opaque", mVertBuff, mIndexBuff, 6, rootData);
}