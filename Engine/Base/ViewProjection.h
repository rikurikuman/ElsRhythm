#pragma once
#include "Matrix4.h"
#include "Util.h"
#include <Vector2.h>
#include "Vector3.h"

struct ViewProjectionBuffer
{
	Matrix4 matrix;
	Vector3 cameraPos;
};

class ViewProjection
{
public:
	enum class Type {
		Orthographic,
		Perspective
	};
	Type mType = Type::Perspective;
	Matrix4 mView;
	Matrix4 mProjection;
	Matrix4 mMatrix;
	Vector3 mEye = {0, 0, 0}; //���_���W
	Vector3 mTarget = {0, 0, 1}; //�����_���W
	Vector3 mUpVec = {0, 1, 0}; //������x�N�g��

	Vector2 mOrthoSize = { 1, 1 };

	float mFov = Util::AngleToRadian(45); //��p(Rad)
	float mAspect = 1; //�A�X�y�N�g��(��/�c)
	float mNearclip = 0.3f; //�j�A�N���b�v����
	float mFarclip = 1000; //�t�@�[�N���b�v����

	//�����o�ϐ��̏��Ŋe�s��𐶐�����
	void UpdateMatrix();

	/// <summary>
	/// �萔�o�b�t�@�֓]��
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	void Transfer(ViewProjectionBuffer* target);

	Vector2 WorldToScreen(Vector3 wPos);
	Vector2 WorldToScreen(Vector3 wPos, float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth);
};