#pragma once
#include "Obj3D.h"
#include "Texture.h"
#include "Vector2.h"
#include "Image3D.h"

class BillboardImage : public Obj3D
{
public:
	Image3D image;
	bool billboardY = false;

	BillboardImage() {};
	BillboardImage(TextureHandle texture, Vector2 size = {1, 1})
	{
		Init(texture, size);
	}

	//����������
	void Init(TextureHandle texture, Vector2 size);

	void Update(const ViewProjection& vp);

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��v����Renderer��
	void Draw();

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override;
};

