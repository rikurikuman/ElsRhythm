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

	/// <summary>
	/// �`��R�}���h��Renderer�ɗv�����܂�
	/// ���̊֐������s�����u�Ԃɕ`��R�}���h���R�[�������킯�ł͂Ȃ����Ƃɒ��ӂ��Ă�������
	/// </summary>
	/// <param name="stageID">�`��X�e�[�W��</param>
	/// <param name="vertView">���_�o�b�t�@�r���[</param>
	/// <param name="indexView">�C���f�b�N�X�o�b�t�@�r���[</param>
	/// <param name="indexCount">�C���f�b�N�X�̐�</param>
	/// <param name="rootData">���[�g�p�����[�^�[</param>
	/// <param name="anchorPoint">�`��D��x��������Ɏg�����W</param>
	static void DrawCall(std::string stageID, D3D12_VERTEX_BUFFER_VIEW* vertView, D3D12_INDEX_BUFFER_VIEW* indexView, uint32_t indexCount, const std::vector<RootData>& rootData, const Vector3& anchorPoint = Vector3::ZERO);
	
	/// <summary>
	/// �`��R�}���h��Renderer�ɗv�����܂�
	/// ���̊֐������s�����u�Ԃɕ`��R�}���h���R�[�������킯�ł͂Ȃ����Ƃɒ��ӂ��Ă�������
	/// </summary>
	/// <param name="stageID">�`��X�e�[�W��</param>
	/// <param name="vertView">���_�o�b�t�@�r���[</param>
	/// <param name="indexView">�C���f�b�N�X�o�b�t�@�r���[</param>
	/// <param name="indexCount">�C���f�b�N�X�̐�</param>
	/// <param name="rootData">���[�g�p�����[�^�[</param>
	/// <param name="anchorPoint">�`��D��x��������Ɏg�����W</param>
	static void DrawCall(std::string stageID, SRVertexBuffer& vertBuff, SRIndexBuffer& indexBuff, uint32_t indexCount, const std::vector<RootData>& rootData, const Vector3& anchorPoint = Vector3::ZERO);
	
	/// <summary>
	/// �`��R�}���h��Renderer�ɗv�����܂�
	/// </summary>
	/// <param name="stageID">�`��X�e�[�W��</param>
	/// <param name="vertView">���_�o�b�t�@�r���[</param>
	/// <param name="indexView">�C���f�b�N�X�o�b�t�@�r���[</param>
	/// <param name="indexCount">�C���f�b�N�X�̐�</param>
	/// <param name="rootData">���[�g�p�����[�^�[</param>
	/// <param name="anchorPoint">�`��D��x��������Ɏg�����W</param>
	static void DrawCall(std::string stageID, RenderOrder order);

	//RenderStage���w��ID��RenderStage�̑O�ɒǉ����܂�
	template<class T>
	static void AddRenderStageFront(std::string targetID = "") {
		Renderer* instance = GetInstance();
		if (!targetID.empty()) {
			for (auto itr = instance->mStages.begin(); itr != instance->mStages.end(); itr++) {
				IRenderStage* stage = itr->get();
				if (stage->GetTypeIndentifier() == targetID) {
					std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
					ptr->Init();
					instance->mStages.insert(itr, std::move(ptr));
					return;
				}
			}
		}

		std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
		ptr->Init();
		instance->mStages.insert(instance->mStages.begin(), std::move(ptr));
	}

	//RenderStage���w��ID��RenderStage�̌�ɒǉ����܂�
	template<class T>
	static void AddRenderStageBack(std::string targetID = "") {
		Renderer* instance = GetInstance();
		if (!targetID.empty()) {
			for (auto itr = instance->mStages.begin(); itr != instance->mStages.end(); itr++) {
				IRenderStage* stage = itr->get();
				if (stage->GetTypeIndentifier() == targetID) {
					itr++;
					std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
					ptr->Init();
					instance->mStages.insert(itr, std::move(ptr));
					return;
				}
			}
		}

		std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
		ptr->Init();
		instance->mStages.push_back(std::move(ptr));
	}

	//�w��ID��RenderStage���폜���܂�
	static void RemoveRenderStage(std::string id);

	//�w��ID��RenderStage���擾���܂�
	static IRenderStage* GetRenderStage(std::string id);

	//����̕`��Ɏg��PrimitiveTopology���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
		GetInstance()->mPrimitiveTopology = topology;
	}

	//����̕`��Ɏg��RenderTarget���o�b�N�o�b�t�@�Ɏw�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetToBackBuffer() {
		GetInstance()->mRenderTargets.clear();
	}

	//����̕`��Ɏg��RenderTarget�̌����Renderer�ɔC���܂�
	static void SetRenderTargetToAuto() {
		GetInstance()->mRenderTargets.clear();
	}

	//����̕`��Ɏg��RenderTarget���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetRenderTarget(std::string renderTargetName) {
		GetInstance()->mRenderTargets = std::vector<std::string>{ renderTargetName };
	}

	//����̕`��Ɏg��RenderTarget���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetRenderTargets(std::vector<std::string> renderTargetNames) {
		GetInstance()->mRenderTargets = renderTargetNames;
	}

	//����̕`��Ɏg��RootSignature�̌����Renderer�ɔC���܂�
	static void SetRootSignatureToAuto() {
		GetInstance()->mRootSignature = nullptr;
	}

	//����̕`��Ɏg��RootSignature���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetRootSignature(const RootSignature& mRootSignature) {
		GetInstance()->mRootSignature = mRootSignature.mPtr.Get();
	}

	//����̕`��Ɏg��PipelineState�̌����Renderer�ɔC���܂�
	static void SetPipelineToAuto() {
		GetInstance()->mPipelineState = nullptr;
	}

	//����̕`��Ɏg��PipelineState���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetPipeline(const GraphicsPipeline& pipeline) {
		GetInstance()->mPipelineState = pipeline.mPtr.Get();
	}

	//����̕`��Ɏg��Viewport���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetViewports(const std::vector<Viewport>& viewports) {
		GetInstance()->mViewports = viewports;
	}

	//����̕`��Ɏg��Viewport���w�肵�܂�
	//��x�Z�b�g����Ƃ��̌ジ���ƈێ�����܂�
	//�ꎞ�I�ɕς���ꍇ�Ȃǂɖ߂��Y��Ȃ��悤����
	static void SetScissorRects(const std::vector<RRect> scissorRects) {
		GetInstance()->mScissorRects = scissorRects;
	}

	//����̕`��Ɏg���p�����[�^��Renderer�ɔC���܂�
	static void SetAllParamaterToAuto();

private:
	Renderer() {
		Init();
	};
	~Renderer() = default;
	Renderer(const Renderer&) {};
	Renderer& operator=(const Renderer&) { return *this; }

	void Init();

	template<class T>
	void AddDefRenderStageBack() {
		std::unique_ptr<IRenderStage> ptr = std::make_unique<T>();
		ptr->Init();
		mStages.push_back(std::move(ptr));
	}

	std::vector<std::unique_ptr<IRenderStage>> mStages;

	std::vector<std::string> mRenderTargets;
	D3D_PRIMITIVE_TOPOLOGY mPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ID3D12RootSignature* mRootSignature = nullptr;
	ID3D12PipelineState* mPipelineState = nullptr;
	std::vector<Viewport> mViewports;
	std::vector<RRect> mScissorRects;
};

