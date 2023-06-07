#pragma once
#include "Vector3.h"
#include "Matrix4.h"

class Quaternion {
public:
	float x;
	float y;
	float z;
	float w;

	//�P�ʉ�]�̃N�H�[�^�j�I��
	//����́u��]���Ă��Ȃ��v�̂Ɠ����ł���
	Quaternion();

	//�e�l���w�肵�ăN�H�[�^�j�I���̃C���X�^���X�����
	//�ʏ�A���̃R���X�g���N�^���e�l�𗝉����Ă��Ȃ��҂��ĂԂׂ��ł͂Ȃ�
	Quaternion(float x, float y, float z, float w);

	//�N�H�[�^�j�I�����m���݂��Ɉ�v���邩��r����
	//����͓�̓��ς�1�ɋ߂������e�X�g����
	bool operator==(const Quaternion& rq) const;

	//�N�H�[�^�j�I���̘a
	Quaternion operator+(const Quaternion& rq) const;

	//�N�H�[�^�j�I���̍�
	Quaternion operator-(const Quaternion& rq) const;

	//�X�J���[�{
	Quaternion operator*(const float& f) const;

	//�X�J���[����1�{
	Quaternion operator/(const float& f) const;

	//�N�H�[�^�j�I���̘a
	Quaternion operator+=(const Quaternion& rq);

	//�N�H�[�^�j�I���̍�
	Quaternion operator-=(const Quaternion& rq);

	//�X�J���[�{
	Quaternion& operator*=(const float& f);

	//�X�J���[����1�{
	Quaternion& operator/=(const float& f);

	//�N�H�[�^�j�I���̑S�Ă̒l�̕����𔽓]������
	//��]���Ɖ�]�ʑS�Ă��t�ɂȂ邽�߁A��]��̌��ʂƂ��Ă͓�����]��\��
	//�t��]���~�����Ȃ�Conjugate�֐�
	//�t�����~�����Ȃ�Inverse�֐�
	Quaternion operator-() const;

	//��]����������
	//������ł���Aa * b��b * a�͓�����]�ł͂Ȃ�
	//a * b��b�̉�]�̌��a�̉�]���s���̂Ɠ����A����
	Quaternion operator*(const Quaternion& rq) const;

	//��]����������
	//������ł���Aa * b��b * a�͓�����]�ł͂Ȃ�
	//a * b��b�̉�]�̌��a�̉�]���s���̂Ɠ����A����
	//���̊֐���a = b * a;������̂Ɠ���
	Quaternion& operator*=(const Quaternion& rq);

	//���̃N�H�[�^�j�I���̋t���𓾂�
	//���̃C���X�^���X������������
	Quaternion& Inverse();

	//���̃N�H�[�^�j�I���̋t���𓾂�
	//���̃C���X�^���X�͏����������A�ʂ̃C���X�^���X�Ƃ��ē���
	Quaternion GetInverse() const;

	//���̃N�H�[�^�j�I���̋t��](����)�𓾂�
	//���̃C���X�^���X������������
	Quaternion& Conjugate();

	//���̃N�H�[�^�j�I���̋t��](����)�𓾂�
	//���̃C���X�^���X�͏����������A�ʂ̃C���X�^���X�Ƃ��ē���
	Quaternion GetConjugate() const;

	//���̃N�H�[�^�j�I���𐳋K������
	//���ʂɎg������́A��{�I�ɌĂԕK�v�͂Ȃ�
	Quaternion& Normalize();

	//�m�����𓾂�
	//���ʎg��Ȃ�
	float GetNorm() const;

	float Dot(const Quaternion& q) const;

	//�I�C���[�p�ɕϊ�����
	Vector3 ToEuler() const;

	//��]�s��ɕϊ�����
	Matrix4 ToRotationMatrix() const;

	//�ȉ��֗��֐��H

	/// <summary>
	/// �I�C���[�p�ŕ\���ꂽ��]���N�H�[�^�j�I���ŕ\��
	/// </summary>
	/// <param name="x">x������̉�]</param>
	/// <param name="y">y������̉�]</param>
	/// <param name="z">z������̉�]</param>
	/// <returns>��]�N�H�[�^�j�I��</returns>
	static Quaternion Euler(const float& x, const float& y, const float& z);

	/// <summary>
	/// �I�C���[�p�ŕ\���ꂽ��]���N�H�[�^�j�I���ŕ\��
	/// </summary>
	/// <param name="rot">�I�C���[�p</param>
	/// <returns>��]�N�H�[�^�j�I��</returns>
	static Quaternion Euler(const Vector3& rot);

	/// <summary>
	/// ��]�s�񂩂�N�H�[�^�j�I���𐶐�����
	/// </summary>
	/// <param name="mat">��]�s��</param>
	/// <returns>��]�N�H�[�^�j�I��</returns>
	static Quaternion FromRotationMatrix(const Matrix4& mat);

	/// <summary>
	/// �C�ӂ̉�]���Ŏw�肵���p�x������]����N�H�[�^�j�I���𐶐�����
	/// </summary>
	/// <param name="vec">��]��</param>
	/// <param name="angle">�p�x(Radian)</param>
	/// <returns>��]�N�H�[�^�j�I��</returns>
	static Quaternion AngleAxis(const Vector3& vec, const float& angle);

	/// <summary>
	/// �w�肵��������������]�N�H�[�^�j�I���𐶐�����
	/// </summary>
	/// <param name="vec">��������������</param>
	/// <param name="vec">������x�N�g��</param>
	/// <returns>��]�N�H�[�^�j�I��</returns>
	static Quaternion LookAt(const Vector3& vec, const Vector3& up = {0, 1, 0});

	/// <summary>
	/// from����to�ւ̉�]�N�H�[�^�j�I���𐶐�����
	/// </summary>
	/// <param name="from">��]�O�̎p���N�H�[�^�j�I��</param>
	/// <param name="to">��]��̎p���N�H�[�^�j�I��</param>
	/// <returns>��]�N�H�[�^�j�I��</returns>
	static Quaternion FromToRotation(const Quaternion& from, const Quaternion& to);

	/// <summary>
	/// ��̃N�H�[�^�j�I������`��Ԃ���
	/// t=0��qA,t=1��qB�Ɉ�v����
	/// </summary>
	/// <param name="qA">�n�_�N�H�[�^�j�I��</param>
	/// <param name="qB">�I�_�N�H�[�^�j�I��</param>
	/// <param name="t">����(����)</param>
	/// <returns>��Ԃ��ꂽ�N�H�[�^�j�I��(���K���ς�)</returns>
	static Quaternion Lerp(const Quaternion& qA, const Quaternion& qB, const float& t);

	/// <summary>
	/// ��̃N�H�[�^�j�I�������ʐ��`��Ԃ���
	/// t=0��qA,t=1��qB�Ɉ�v����
	/// </summary>
	/// <param name="qA">�n�_�N�H�[�^�j�I��</param>
	/// <param name="qB">�I�_�N�H�[�^�j�I��</param>
	/// <param name="t">����(����)</param>
	/// <returns>��Ԃ��ꂽ�N�H�[�^�j�I��(���K���ς�)</returns>
	static Quaternion Slerp(const Quaternion& qA, const Quaternion& qB, const float& t);
};

//�X�J���[�{
Quaternion operator*(const float& f, const Quaternion& q);

//�X�J���[�{
Quaternion& operator*=(const float& f, Quaternion& q);

//�x�N�g�����N�H�[�^�j�I���ŉ�
Vector3 operator*(const Vector3& v, const Quaternion& q);

//�x�N�g�����N�H�[�^�j�I���ŉ�
Vector3& operator*=(Vector3& v, const Quaternion& q);