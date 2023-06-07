#pragma once
#include "Matrix4.h"
#include "Vector3.h"

struct TransformBuffer
{
	Matrix4 matrix;
};

class Transform
{
public:
	Transform* parent = nullptr;
	Vector3 position = { 0, 0, 0 };
	Vector3 rotation = { 0, 0, 0 };
	Vector3 scale = { 1, 1, 1 };

	Matrix4 matrix;

	//pos(0, 0, 0), rot(0, 0, 0), scale(1, 1, 1)��Transform�𐶐�
	Transform() {
		UpdateMatrix();
	}

	Transform(Vector3 position, Vector3 rotation, Vector3 scale) : position(position), rotation(rotation), scale(scale) {
		UpdateMatrix();
	}

	//�����o�ϐ���3���Ń��[���h���W�ϊ��s��𐶐�����
	void UpdateMatrix();

	/// <summary>
	/// �萔�o�b�t�@�֓]��
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	void Transfer(TransformBuffer* target);

	/// <summary>
	/// �s��Ɗ|���Z���Ă���萔�o�b�t�@�ɓ]��,
	/// �r���[+�ˉe�ϊ��s������Ċy����p
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	/// <param name="matrix">�]���O�Ɋ|����s��</param>
	void Transfer(TransformBuffer* target, Matrix4 matrix);
};

