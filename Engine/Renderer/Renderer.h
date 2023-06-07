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

	//Renderer�ɂ��`�揈�������s���܂�
	static void Execute();

	//�`��R�}���h��ς݂܂�
	static void DrawCall(std::string stageID, D3D12_VERTEX_BUFFER_VIEW* vertView, D3D12_INDEX_BUFFER_VIEW* indexView, UINT indexCount, const std::vector<RootData>& rootData, Vector3& anchorPoint = Vector3(0, 0, 0));
	//�`��R�}���h��ς݂܂�
	static void DrawCall(std::string stageID, SRVertexBuffer& vertBuff, SRIndexBuffer& indexBuff, UINT indexCount, const std::vector<RootData>& rootData, Vector3& anchorPoint = Vector3(0, 0, 0));
	//�`��R�}���h��ς݂܂�
	static void DrawCall(std::string stageID, RenderOrder& order);

	//RenderStage�����������܂�
	static void InitRenderStages();

	//RenderStage���w��ID��RenderStage�̑O�ɒǉ����܂�
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

	//RenderStage���w��ID��RenderStage�̌�ɒǉ����܂�
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

	//�w��ID��RenderStage���폜���܂�
	static void RemoveRenderStage(std::string id);

	//�w��ID��RenderStage���擾���܂�
	static IRenderStage* GetRenderStage(std::string id);

	//����̕`��Ɏg��PrimitiveTopology���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
		GetInstance()->primitiveTopology = topology;
	}

	//����̕`��Ɏg��RenderTarget���o�b�N�o�b�t�@�Ɏw�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetToBackBuffer() {
		GetInstance()->renderTargets.clear();
	}

	//����̕`��Ɏg��RenderTarget�̌����Renderer�ɔC���܂�
	static void SetRenderTargetToAuto() {
		GetInstance()->renderTargets.clear();
	}

	//����̕`��Ɏg��RenderTarget���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetRenderTarget(std::string renderTargetName) {
		GetInstance()->renderTargets = std::vector<std::string>{ renderTargetName };
	}

	//����̕`��Ɏg��RenderTarget���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetRenderTargets(std::vector<std::string> renderTargetNames) {
		GetInstance()->renderTargets = renderTargetNames;
	}

	//����̕`��Ɏg��RootSignature�̌����Renderer�ɔC���܂�
	static void SetRootSignatureToAuto() {
		GetInstance()->rootSignature = nullptr;
	}

	//����̕`��Ɏg��RootSignature���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetRootSignature(const RootSignature& rootSignature) {
		GetInstance()->rootSignature = rootSignature.ptr.Get();
	}

	//����̕`��Ɏg��PipelineState�̌����Renderer�ɔC���܂�
	static void SetPipelineToAuto() {
		GetInstance()->pipelineState = nullptr;
	}

	//����̕`��Ɏg��PipelineState���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetPipeline(const GraphicsPipeline& pipeline) {
		GetInstance()->pipelineState = pipeline.ptr.Get();
	}

	//����̕`��Ɏg��Viewport���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetViewports(const std::vector<Viewport>& viewports) {
		GetInstance()->viewports = viewports;
	}

	//����̕`��Ɏg��Viewport���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetScissorRects(const std::vector<Rect> scissorRects) {
		GetInstance()->scissorRects = scissorRects;
	}

	//����̕`��Ɏg���p�����[�^��Renderer�ɔC���܂�
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

