#include "OpaqueRenderStage.h"
#include <RDirectX.h>

void OpaqueRenderStage::Init()
{
	defParamater.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	defParamater.renderTargets = { "RenderingImage" };
	defParamater.rootSignature = RDirectX::GetDefRootSignature().ptr.Get();
	defParamater.pipelineState = RDirectX::GetDefPipeline().ptr.Get();
}

void OpaqueRenderStage::Render()
{
	AllCall();
}
