#pragma once
#include "Obj3D.h"
#include "Image3D.h"
class Cube : public Obj3D
{
public:
	enum Direction {
		Front, Left, Back, Right, Top, Bottom
	};

	Image3D mFaces[6];
	Vector2 mSize = { 1, 1 };

	Cube() {};
	Cube(TextureHandle texture, Vector2 size = {1, 1}, bool forceSize = false);

	/// <summary>
	/// �w�肵���ʂ̃e�N�X�`�����Z�b�g����
	/// </summary>
	/// <param name="texture">�e�N�X�`���n���h��</param>
	/// <param name="direction">�ʂ̌���</param>
	void SetTexture(TextureHandle texture, Direction direction);

	/// <summary>
	/// �S�Ă̖ʂ̃e�N�X�`�����Z�b�g����
	/// </summary>
	/// <param name="texture"></param>
	void SetAllTexture(TextureHandle texture);


	void SetAllAlpha(float alpha);

	//�ʂ�Transform�Ƃ����X�V����
	void UpdateFaces();

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��v����Renderer��
	void Draw() override;

	//�`��p�̃R�}���h���܂Ƃ߂ăR�}���h���X�g�ɐς�
	void DrawCommands() override;
};

