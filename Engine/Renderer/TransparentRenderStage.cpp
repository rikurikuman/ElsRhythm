#include <RDirectX.h>
#include "TransparentRenderStage.h"

void TransparentRenderStage::Init()
{
	pipelineState = RDirectX::GetDefPipeline();

	// ラスタライザの設定
	pipelineState.desc.BlendState.AlphaToCoverageEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = true;
	pipelineState.desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	pipelineState.desc.pRootSignature = RDirectX::GetDefRootSignature().ptr.Get();

	pipelineState.Create();

	defParamater.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	defParamater.renderTargets = { "RenderingImage" };
	defParamater.rootSignature = RDirectX::GetDefRootSignature().ptr.Get();
	defParamater.pipelineState = pipelineState.ptr.Get();
}

void TransparentRenderStage::Render()
{
	AllCall();
}
