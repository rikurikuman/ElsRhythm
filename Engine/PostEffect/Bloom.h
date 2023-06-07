#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>
class Bloom
{
public:
	TextureHandle texture;

	SRVertexBuffer vertBuff;
	SRIndexBuffer indexBuff;

	Bloom();

	void Draw();

protected:
	static RootSignature& GetRootSignature();
	static GraphicsPipeline& GetGraphicsPipelineA(); //���邳���o
	static GraphicsPipeline& GetGraphicsPipelineB(); //�K�E�X�ڂ���
	static GraphicsPipeline& GetGraphicsPipelineC(); //���Z����
};