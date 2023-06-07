#pragma once
#include "Transform.h"
#include "ViewProjection.h"

class Obj3D
{
protected:
	Obj3D* parent = nullptr;

public:
	virtual ~Obj3D() {}

	Transform transform;

	void SetParent(Obj3D* parent) {
		this->parent = parent;
		this->transform.parent = &parent->transform;
	}

	const Obj3D* GetParent() {
		return parent;
	}

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	virtual void TransferBuffer(ViewProjection viewprojection) {}

	//�`��v����Renderer��
	virtual void Draw() {}

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	virtual void DrawCommands() {}
};

