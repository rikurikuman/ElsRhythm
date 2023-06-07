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

	//�e�N�X�`����ύX����
	void SetTexture(TextureHandle texture);

	void Init();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer();

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands();

	static void InitPipeline();

public:
	static RootSignature rootSignature;
	static GraphicsPipeline pipelineState;
};