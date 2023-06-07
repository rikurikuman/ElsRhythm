#include "BackSpriteRenderStage.h"
#include <algorithm>
#include "Renderer.h"
#include "Sprite.h"
#include <RInput.h>

void BackSpriteRenderStage::Init()
{
	defParamater.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	defParamater.renderTargets = { "RenderingImage" };
	defParamater.rootSignature = SpriteManager::GetInstance()->GetRootSignature().ptr.Get();
	defParamater.pipelineState = SpriteManager::GetInstance()->GetGraphicsPipeline().ptr.Get();
}

void BackSpriteRenderStage::Render()
{
	struct sortObj {
		bool operator()(const RenderOrder& a, const RenderOrder& b) const noexcept {
			return a.anchorPoint.z < b.anchorPoint.z;
		}
	};
	std::stable_sort(orders.begin(), orders.end(), sortObj{});

	AllCall();
}
