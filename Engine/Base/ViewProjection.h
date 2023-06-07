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
	Type type = Type::Perspective;
	Matrix4 view;
	Matrix4 projection;
	Matrix4 matrix;
	Vector3 eye = {0, 0, 0}; //���_���W
	Vector3 target = {0, 0, 1}; //�����_���W
	Vector3 up = {0, 1, 0}; //������x�N�g��

	Vector2 orthoSize = { 1, 1 };

	float fov = Util::AngleToRadian(45); //��p(Rad)
	float aspect = 1; //�A�X�y�N�g��(��/�c)
	float nearclip = 0.1f; //�j�A�N���b�v����
	float farclip = 1000; //�t�@�[�N���b�v����

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