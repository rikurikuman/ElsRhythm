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
	static void DrawLine(int x1, int y1, int x2, int y2, float layer, Color color, float thickness = 1.0f);


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
	static void DrawBox(int x1, int y1, int x2, int y2, float layer, Color color, bool fillFlag, float thickness = 1.0f);

	/// <summary>
	/// 2D��ʂɉ~��`�悷��
	/// </summary>
	/// <param name="x">���S�̃X�N���[�����WX</param>
	/// <param name="y">���S�̃X�N���[�����WY</param>
	/// <param name="layer">�`��D��x</param>
	/// <param name="color">�F</param>
	/// <param name="fillFlag">������h��Ԃ���</param>
	/// <param name="thickness">�h��Ԃ��Ȃ��ꍇ�̐��̑���</param>
	static void DrawCircle(int x, int y, int r, float layer, Color color, bool fillFlag, float thickness = 1.0f);
	
	static void DrawString(float x, float y, float layer, std::string text, Color color = Color(1.0f, 1.0f, 1.0f, 1.0f), std::string fontTypeFace = "", UINT fontSize = 20, Vector2 anchor = {0, 0});

	//��񂵕`��ɂ���Ē񋟂������̂�S���`�悷��
	static void DrawAll();

private:
	SimpleDrawer() {
		Init();
	};
	~SimpleDrawer() = default;
	SimpleDrawer(const SimpleDrawer& a) = default;
	SimpleDrawer& operator=(const SimpleDrawer&) { return *this; }

	std::list<std::shared_ptr<SimpleDrawInfo>> infoList;
	std::unordered_map<std::string, std::shared_ptr<SRBufferPtr>> recycleBuffs;

	std::unordered_map<float, std::vector<DrawBoxInfo>> boxInfoMap;
	std::unordered_map<float, DrawBuffers> boxBuffersMap;
	RootSignature boxRS;
	GraphicsPipeline boxPSO;

	std::unordered_map<float, std::vector<DrawLineInfo>> lineInfoMap;
	std::unordered_map<float, DrawBuffers> lineBuffersMap;
	RootSignature lineRS;
	GraphicsPipeline linePSO;
	SRConstBuffer<ViewProjectionBuffer> lineVPBuff;

	std::unordered_map<DrawCustomData, std::vector<DrawCircleInfo>> circleInfoMap;
	std::unordered_map<DrawCustomData, DrawCircleVertIndex> circleVertIndexMap;
	std::unordered_map<DrawCustomData, DrawBuffers> circleBuffersMap;
	RootSignature circleRS;
	GraphicsPipeline circlePSO;
	SRConstBuffer<ViewProjectionBuffer> circleVPBuff;
	void CalcCircleVertAndIndex(DrawCustomData cData);

	std::unordered_map<float, std::vector<DrawLine3DInfo>> line3DInfoMap;
	std::unordered_map<float, DrawBuffers> line3DBuffersMap;
	RootSignature line3DRS;
	GraphicsPipeline line3DPSO;

	RootSignature rootSignature;
	GraphicsPipeline pipelineState;

	RootSignature rootSignatureForString;
	GraphicsPipeline pipelineStateForString;

	std::mutex mutex;

	void Init();
};

