#pragma once

//���ǂ�����錾
class Vector2;
class Float4;

class Vector3
{
public:
	float x;
	float y;
	float z;

	//�[���x�N�g��
	Vector3();
	const static Vector3 ZERO;

	//�C�ӂ̐����x�N�g��
	Vector3(float x, float y, float z);

	~Vector3();

	//��r
	bool operator==(const Vector3& a) const;
	bool operator!=(const Vector3& a) const;

	//���`�̎㏇��(�x�N�g���̒����������肷��킯����Ȃ��̂ŕ��ʎg��Ȃ��j
	bool operator<(const Vector3& a) const;

	//�x�N�g���̑����Z
	Vector3 operator+(const Vector3& a) const;
	//�x�N�g���̈����Z
	Vector3 operator-(const Vector3& a) const;
	//�t�x�N�g��
	Vector3 operator-() const;
	//�萔�{
	Vector3 operator*(const float a) const;
	//�萔����1�{
	Vector3 operator/(const float a) const;

	//�x�N�g���̑����Z
	Vector3& operator+=(const Vector3& a);
	//�x�N�g���̈����Z
	Vector3& operator-=(const Vector3& a);
	//�x�N�g���̂����Z
	Vector3& operator*=(const Vector3& a);
	//�萔�{
	Vector3& operator*=(const float a);
	//�萔����1�{
	Vector3& operator/=(const float a);

	//����
	float Dot(const Vector3& a) const;
	//�O��
	Vector3 Cross(const Vector3& a) const;

	//�x�N�g���̒����i����g���̂ŏd�߁j
	float Length() const;
	//�x�N�g���ׂ̂��撷���i����g��Ȃ��̂Ōy�߁j
	float LengthSq() const;

	//���K���i���̃C���X�^���X������������j
	Vector3& Normalize();
	//���K���x�N�g�����擾
	//�i���̃C���X�^���X�͏����������ɕʂ̃C���X�^���X�Ƃ��Ď擾����j
	Vector3 GetNormalize() const;

	//z�������̂Ă�Vector2�փL���X�g
	operator Vector2() const;

	//w������1�Ƃ���Float4�փL���X�g
	operator Float4() const;
};

//���ӂ�float��u�����Ƃ���*, /���o���Ȃ��̂ł���΍�

//�萔�{
Vector3 operator*(const float a, const Vector3& b);

//���`���(1���֐����)
const Vector3 lerp(const Vector3& start, const Vector3& end, const float t);