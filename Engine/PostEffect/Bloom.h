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
	static GraphicsPipeline& GetGraphicsPipelineA(); //明るさ抽出
	static GraphicsPipeline& GetGraphicsPipelineB(); //ガウスぼかし
	static GraphicsPipeline& GetGraphicsPipelineC(); //加算合成
};