#pragma once
#include <vector>
#include <memory>
#include "IRenderStage.h"
#include "RenderOrder.h"

class Renderer
{

public:
	static Renderer* GetInstance() {
		static Renderer instance;
		return &instance;
	}

	//Rendererによる描画処理を実行します
	static void Execute();

	//描画コマンドを積みます
	static void DrawCall(std::string stageID, D3D12_VERTEX_BUFFER_VIEW* vertView, D3D12_INDEX_BUFFER_VIEW* indexView, UINT indexCount, const std::vector<RootData>& rootData, Vector3& anchorPoint = Vector3(0, 0, 0));
	//描画コマンドを積みます
	static void DrawCall(std::string stageID, SRVertexBuffer& vertBuff, SRIndexBuffer& indexBuff, UINT indexCount, const std::vector<RootData>& rootData, Vector3& anchorPoint = Vector3(0, 0, 0));
	//描画コマンドを積みます
	static void DrawCall(std::string stageID, RenderOrder& order);

	//RenderStageを初期化します
	static void InitRenderStages();

	//RenderStageを指定IDのRenderStageの前に追加します
	template<class T>
	static void AddRenderStageFront(std::string targetID = "") {
		Renderer* instance = GetInstance();
		if (!targetID.empty()) {
			for (auto itr = instance->stages.begin(); itr != instance->stages.end(); itr++) {
				IRenderStage* stage = itr->get();
				if (stage->GetTypeIndentifier() == targetID) {
					std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
					ptr->Init();
					instance->stages.insert(itr, std::move(ptr));
					return;
				}
			}
		}

		std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
		ptr->Init();
		instance->stages.insert(instance->stages.begin(), std::move(ptr));
	}

	//RenderStageを指定IDのRenderStageの後に追加します
	template<class T>
	static void AddRenderStageBack(std::string targetID = "") {
		Renderer* instance = GetInstance();
		if (!targetID.empty()) {
			for (auto itr = instance->stages.begin(); itr != instance->stages.end(); itr++) {
				IRenderStage* stage = itr->get();
				if (stage->GetTypeIndentifier() == targetID) {
					itr++;
					std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
					ptr->Init();
					instance->stages.insert(itr, std::move(ptr));
					return;
				}
			}
		}

		std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
		ptr->Init();
		instance->stages.push_back(std::move(ptr));
	}

	//指定IDのRenderStageを削除します
	static void RemoveRenderStage(std::string id);

	//指定IDのRenderStageを取得します
	static IRenderStage* GetRenderStage(std::string id);

	//今後の描画に使うPrimitiveTopologyを指定します
	//一度セットするとその後ずっと維持されます
	//一時的に変える場合などに戻し忘れないよう注意
	static void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
		GetInstance()->primitiveTopology = topology;
	}

	//今後の描画に使うRenderTargetをバックバッファに指定します
	//一度セットするとその後ずっと維持されます
	//一時的に変える場合などに戻し忘れないよう注意
	static void SetToBackBuffer() {
		GetInstance()->renderTargets.clear();
	}

	//今後の描画に使うRenderTargetの決定をRendererに任せます
	static void SetRenderTargetToAuto() {
		GetInstance()->renderTargets.clear();
	}

	//今後の描画に使うRenderTargetを指定します
	//一度セットするとその後ずっと維持されます
	//一時的に変える場合などに戻し忘れないよう注意
	static void SetRenderTarget(std::string renderTargetName) {
		GetInstance()->renderTargets = std::vector<std::string>{ renderTargetName };
	}

	//今後の描画に使うRenderTargetを指定します
	//一度セットするとその後ずっと維持されます
	//一時的に変える場合などに戻し忘れないよう注意
	static void SetRenderTargets(std::vector<std::string> renderTargetNames) {
		GetInstance()->renderTargets = renderTargetNames;
	}

	//今後の描画に使うRootSignatureの決定をRendererに任せます
	static void SetRootSignatureToAuto() {
		GetInstance()->rootSignature = nullptr;
	}

	//今後の描画に使うRootSignatureを指定します
	//一度セットするとその後ずっと維持されます
	//一時的に変える場合などに戻し忘れないよう注意
	static void SetRootSignature(const RootSignature& rootSignature) {
		GetInstance()->rootSignature = rootSignature.ptr.Get();
	}

	//今後の描画に使うPipelineStateの決定をRendererに任せます
	static void SetPipelineToAuto() {
		GetInstance()->pipelineState = nullptr;
	}

	//今後の描画に使うPipelineStateを指定します
	//一度セットするとその後ずっと維持されます
	//一時的に変える場合などに戻し忘れないよう注意
	static void SetPipeline(const GraphicsPipeline& pipeline) {
		GetInstance()->pipelineState = pipeline.ptr.Get();
	}

	//今後の描画に使うViewportを指定します
	//一度セットするとその後ずっと維持されます
	//一時的に変える場合などに戻し忘れないよう注意
	static void SetViewports(const std::vector<Viewport>& viewports) {
		GetInstance()->viewports = viewports;
	}

	//今後の描画に使うViewportを指定します
	//一度セットするとその後ずっと維持されます
	//一時的に変える場合などに戻し忘れないよう注意
	static void SetScissorRects(const std::vector<Rect> scissorRects) {
		GetInstance()->scissorRects = scissorRects;
	}

	//今後の描画に使うパラメータをRendererに任せます
	static void SetAllParamaterToAuto();

private:
	Renderer() {
		Init();
	};
	~Renderer() = default;
	Renderer(const Renderer& a) {};
	Renderer& operator=(const Renderer&) { return *this; }

	void Init();

	template<class T>
	void AddDefRenderStageBack() {
		std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
		ptr->Init();
		stages.push_back(std::move(ptr));
	}

	std::vector<std::unique_ptr<IRenderStage>> stages;

	std::vector<std::string> renderTargets;
	D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* pipelineState = nullptr;
	std::vector<Viewport> viewports;
	std::vector<Rect> scissorRects;
};

