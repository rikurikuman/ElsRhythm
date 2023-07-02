#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>
#include <RenderTarget.h>

class CrossFilter
{
public:
	struct BlurSetting {
		float angle = 0;
		float pickRange = 0.06f;
	};

	TextureHandle mTexture;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<BlurSetting> mConstBuff;

	BlurSetting mSetting{};

	CrossFilter();

	void Draw();

protected:
	RenderTexture* texA;
	RenderTexture* texB;

	static RootSignature& GetRootSignatureA();
	static RootSignature& GetRootSignatureB();
	static GraphicsPipeline& GetGraphicsPipelineA(); //���邳���o
	static GraphicsPipeline& GetGraphicsPipelineB(); //������ڂ���
	static GraphicsPipeline& GetGraphicsPipelineC(); //���Z����
};