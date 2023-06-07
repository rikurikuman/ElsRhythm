#include "PostEffectRenderStage.h"
#include <RDirectX.h>
#include <algorithm>

void PostEffectRenderStage::Init()
{
	defParamater.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	defParamater.renderTargets = { "RenderingImage" };
	defParamater.rootSignature = RDirectX::GetDefRootSignature().ptr.Get();
	defParamater.pipelineState = RDirectX::GetDefPipeline().ptr.Get();
}

void PostEffectRenderStage::Render()
{
	struct sortObj {
		bool operator()(const RenderOrder& a, const RenderOrder& b) const noexcept {
			return a.anchorPoint.z < b.anchorPoint.z;
		}
	};
	std::stable_sort(orders.begin(), orders.end(), sortObj{});

	AllCall();
}
