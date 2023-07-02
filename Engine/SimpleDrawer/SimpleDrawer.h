#pragma once
#include "RDirectX.h"
#include "SimpleDrawInfo.h"
#include <mutex>

class SimpleDrawer
{
public:
	static SimpleDrawer* GetInstance() {
		static SimpleDrawer instance;
		return &instance;
	}

	static void ClearData();

	/// <summary>
	/// 2D��ʂɐ���`�悷��
	/// </summary>
	/// <param name="x1">�J�n�ʒu�̃X�N���[�����WX</param>
	/// <param name="y1">�J�n�ʒu�̃X�N���[�����WY</param>
	/// <param name="x2">�I���ʒu�̃X�N���[�����WX</param>
	/// <param name="y2">�I���ʒu�̃X�N���[�����WY</param>
	/// <param name="layer">�`��D��x</param>
	/// <param name="color">�F</param>
	/// <param name="thickness">���̑���</param>
	static void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, float layer, Color color, float thickness = 1.0f);


	/// <summary>
	/// 2D��ʂɎl�p�`��`�悷��
	/// </summary>
	/// <param name="x1">����̃X�N���[�����WX</param>
	/// <param name="y1">����̃X�N���[�����WY</param>
	/// <param name="x2">�E���̃X�N���[�����WX</param>
	/// <param name="y2">�E���̃X�N���[�����WY</param>
	/// <param name="layer">�`��D��x</param>
	/// <param name="color">�F</param>
	/// <param name="fillFlag">������h��Ԃ���</param>
	/// <param name="thickness">�h��Ԃ��Ȃ��ꍇ�̐��̑���</param>
	static void DrawBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, float layer, Color color, bool fillFlag, float thickness = 1.0f);

	/// <summary>
	/// 2D��ʂɉ~��`�悷��
	/// </summary>
	/// <param name="x">���S�̃X�N���[�����WX</param>
	/// <param name="y">���S�̃X�N���[�����WY</param>
	/// <param name="layer">�`��D��x</param>
	/// <param name="color">�F</param>
	/// <param name="fillFlag">������h��Ԃ���</param>
	/// <param name="thickness">�h��Ԃ��Ȃ��ꍇ�̐��̑���</param>
	static void DrawCircle(int32_t x, int32_t y, int32_t r, float layer, Color color, bool fillFlag, float thickness = 1.0f);
	
	static void DrawString(float x, float y, float layer, std::string text, Color color = Color(1.0f, 1.0f, 1.0f, 1.0f), std::string fontTypeFace = "", uint32_t fontSize = 20, Vector2 anchor = {0, 0});

	//��񂵕`��ɂ���Ē񋟂������̂�S���`�悷��
	static void DrawAll();

private:
	SimpleDrawer() {
		Init();
	};
	~SimpleDrawer() = default;
	SimpleDrawer(const SimpleDrawer& a) = default;
	SimpleDrawer& operator=(const SimpleDrawer&) { return *this; }

	std::list<std::shared_ptr<SimpleDrawInfo>> mInfoList;
	std::unordered_map<std::string, std::shared_ptr<SRBufferPtr>> mRecycleBuffs;

	std::unordered_map<float, std::vector<DrawBoxInfo>> mBoxInfoMap;
	std::unordered_map<float, DrawBuffers> mBoxBuffersMap;
	RootSignature mBoxRS;
	GraphicsPipeline mBoxPSO;

	std::unordered_map<float, std::vector<DrawLineInfo>> mLineInfoMap;
	std::unordered_map<float, DrawBuffers> mLineBuffersMap;
	RootSignature mLineRS;
	GraphicsPipeline mLinePSO;
	SRConstBuffer<ViewProjectionBuffer> mLineVPBuff;

	std::unordered_map<DrawCustomData, std::vector<DrawCircleInfo>> mCircleInfoMap;
	std::unordered_map<DrawCustomData, DrawCircleVertIndex> mCircleVertIndexMap;
	std::unordered_map<DrawCustomData, DrawBuffers> mCircleBuffersMap;
	RootSignature mCircleRS;
	GraphicsPipeline mCirclePSO;
	SRConstBuffer<ViewProjectionBuffer> mCircleVPBuff;
	void CalcCircleVertAndIndex(DrawCustomData cData);

	std::unordered_map<float, std::vector<DrawLine3DInfo>> mLine3DInfoMap;
	std::unordered_map<float, DrawBuffers> mLine3DBuffersMap;
	RootSignature mLine3DRS;
	GraphicsPipeline mLine3DPSO;

	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;

	RootSignature mRootSignatureForString;
	GraphicsPipeline mPipelineStateForString;

	std::mutex mMutex;

	void Init();
};

