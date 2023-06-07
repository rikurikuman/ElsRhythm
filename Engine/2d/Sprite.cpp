#include "Sprite.h"
#include <d3d12.h>
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"
#include "Renderer.h"

using namespace Microsoft::WRL;

Sprite::Sprite()
{
	Init();
}

Sprite::Sprite(TextureHandle texture, Vector2 anchor)
{
	this->texture = texture;

	//�T�C�Y���Z�b�g����
	
	this->size.x = (float)TextureManager::Get(texture).resource->GetDesc().Width;
	this->size.y = (float)TextureManager::Get(texture).resource->GetDesc().Height;

	//�A���J�[�|�C���g���Z�b�g����
	this->anchor = anchor;

	Init();
}

void Sprite::UpdateVertex()
{
	Vector2 texSize = {
		static_cast<float>(TextureManager::Get(texture).resource->GetDesc().Width),
		static_cast<float>(TextureManager::Get(texture).resource->GetDesc().Height)
	};

	float uvLeft = srcPos.x / texSize.x;
	float uvRight = (srcPos.x + size.x) / texSize.x;
	float uvTop = srcPos.y / texSize.y;
	float uvBottom = (srcPos.y + size.y) / texSize.y;

	//���_�f�[�^
	VertexPNU vertices[] = {
		{{ -anchor.x * size.x, (1 - anchor.y) * size.y, 0.0f}, {}, {uvLeft, uvBottom}}, //����
		{{ -anchor.x * size.x, -anchor.y * size.y, 0.0f }, {}, {uvLeft, uvTop}}, //����
		{{ (1 - anchor.x) * size.x, (1 - anchor.y) * size.y, 0.0f }, {}, {uvRight, uvBottom}}, //�E��
		{{ (1 - anchor.x) * size.x, -anchor.y * size.y, 0.0f }, {}, {uvRight, uvTop}}, //�E��
	};

	vertBuff.Update(vertices, _countof(vertices));
}

void Sprite::SetTexture(TextureHandle texture)
{
	this->texture = texture;
	srcPos = { 0, 0 };
	this->size.x = (float)TextureManager::Get(texture).resource->GetDesc().Width;
	this->size.y = (float)TextureManager::Get(texture).resource->GetDesc().Height;
	change = true;
}

void Sprite::SetAnchor(Vector2 anchor)
{
	this->anchor = anchor;
	change = true;
}

void Sprite::SetTexRect(int srcX, int srcY, int width, int height)
{
	srcPos = { static_cast<float>(srcX), static_cast<float>(srcY) };
	size = { static_cast<float>(width), static_cast<float>(height) };
	change = true;
}

void Sprite::Init()
{
	//���_�f�[�^
	VertexPNU vertices[] = {
		{{ -anchor.x * size.x, (1 - anchor.y) * size.y, 0.0f}, {}, {0.0f, 1.0f}}, //����
		{{ -anchor.x * size.x, -anchor.y * size.y, 0.0f }, {}, {0.0f, 0.0f}}, //����
		{{ (1 - anchor.x) * size.x, (1 - anchor.y) * size.y, 0.0f }, {}, {1.0f, 1.0f}}, //�E��
		{{ (1 - anchor.x) * size.x, -anchor.y * size.y, 0.0f }, {}, {1.0f, 0.0f}}, //�E��
	};

	//���_�C���f�b�N�X�f�[�^
	UINT indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	VertexPNU::CalcNormalVec(vertices, indices, _countof(indices));

	vertBuff.Init(vertices, _countof(vertices));
	indexBuff.Init(indices, _countof(indices));
}

void Sprite::TransferBuffer()
{
	if (change) {
		UpdateVertex();
	}

	material.Transfer(materialBuff.Get());
	transform.Transfer(transformBuff.Get());

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);
	
	viewProjectionBuff.Get()->matrix = matProjection;
}

void Sprite::Draw()
{
	std::vector<RootData> rootData{
		{TextureManager::Get(texture).gpuHandle},
		{RootDataType::SRBUFFER_CBV, materialBuff.buff},
		{RootDataType::SRBUFFER_CBV, transformBuff.buff},
		{RootDataType::SRBUFFER_CBV, viewProjectionBuff.buff},
	};
	
	if (transform.position.z >= 0) {
		Renderer::DrawCall("Sprite", vertBuff, indexBuff, 6, rootData, transform.position);
	}
	else {
		transform.position.z *= -1;
		Renderer::DrawCall("BackSprite", vertBuff, indexBuff, 6, rootData, transform.position);
	}
}

void SpriteManager::Init()
{
	rootSignature = RDirectX::GetDefRootSignature();

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //���j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f; //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //�s�N�Z���V�F�[�_�[���炾��������
	rootSignature.desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	rootSignature.Create();

	pipelineState = RDirectX::GetDefPipeline();

	pipelineState.desc.VS = Shader("./Shader/SpriteVS.hlsl", "main", "vs_5_0");
	pipelineState.desc.PS = Shader("./Shader/SpritePS.hlsl", "main", "ps_5_0");

	// ���X�^���C�U�̐ݒ�
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;
	pipelineState.desc.BlendState.AlphaToCoverageEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = false;
	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();
}
