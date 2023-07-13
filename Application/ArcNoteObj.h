#pragma once
#include "Obj3D.h"
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <Material.h>

class ArcNoteObj : public Obj3D
{
private:
	std::vector<VertexPNU> mVertices;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<TransformBuffer> mTransformBuff;
	SRConstBuffer<MaterialBuffer> mMaterialBuff;

public:
	Vector3 mStartPos;
	Vector3 mGoalPos;
	Material mMaterial;

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��v����Renderer��
	void Draw() override;

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override {}
};

