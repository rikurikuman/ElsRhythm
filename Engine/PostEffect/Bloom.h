#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>

class Bloom
{
public:
	struct BlurSetting {
		float sigma = 0.002f;
		float stepwidth = 0.001f;
	};

	TextureHandle mTexture;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<BlurSetting> mConstBuff;

	BlurSetting mSetting{};
	uint32_t mLevel = 3;

	Bloom();

	void Draw();

protected:
	static RootSignature& GetRootSignatureA();
	static RootSignature& GetRootSignatureB();
	static GraphicsPipeline& GetGraphicsPipelineA(); //���邳���o
	static GraphicsPipeline& GetGraphicsPipelineB(); //�K�E�X�ڂ���
	static GraphicsPipeline& GetGraphicsPipelineC(); //���Z����
};