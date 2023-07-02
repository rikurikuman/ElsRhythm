#pragma once
#include "Transform.h"
#include "ViewProjection.h"

class Obj3D
{
protected:
	Obj3D* mParent = nullptr;

public:
	virtual ~Obj3D() {}

	Transform mTransform;

	void SetParent(Obj3D* parent) {
		mParent = parent;
		mTransform.parent = &parent->mTransform;
	}

	const Obj3D* GetParent() {
		return mParent;
	}

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	virtual void TransferBuffer(ViewProjection viewprojection) {}

	//�`��v����Renderer��
	virtual void Draw() {}

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	virtual void DrawCommands() {}
};

