#pragma once
//���ǂ�����錾
class Vector2;
class Vector3;

class Float4
{
public:
	float x;
	float y;
	float z;
	float w;

	Float4() : x(0), y(0), z(0), w(0) {}
	Float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	//z,w�������̂Ă�Vector2�փL���X�g
	operator Vector2() const;

	//w�������̂Ă�Vector3�փL���X�g
	operator Vector3() const;

	Float4 operator/(const float& f) const;
	Float4& operator/=(const float& f);
};