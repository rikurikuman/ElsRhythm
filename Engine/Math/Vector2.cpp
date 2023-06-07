#include "Vector2.h"
#include <math.h>
#include "Vector3.h"
#include "Float4.h"

const float EPSILON = 0.000001f;

Vector2::Vector2() : x(0), y(0) {}

Vector2::Vector2(int x, int y) :
	x(static_cast<float>(x)),
	y(static_cast<float>(y)) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2::~Vector2() {}

bool Vector2::operator==(const Vector2& a) const {
	return this->x == a.x && this->y == a.y;
}

bool Vector2::operator!=(const Vector2& a) const {
	return this->x != a.x || this->y != a.y;
}

Vector2 Vector2::operator+(const Vector2& a) const {
	return Vector2(this->x + a.x, this->y + a.y);
}

Vector2 Vector2::operator-(const Vector2& a) const {
	return Vector2(this->x - a.x, this->y - a.y);
}

Vector2 Vector2::operator-() const {
	return Vector2(-this->x, -this->y);
}

Vector2 Vector2::operator*(const float a) const {
	return Vector2(this->x * a, this->y * a);
}

Vector2 operator*(const float a, const Vector2& b) {
	return Vector2(b.x * a, b.y * a);
}

Vector2 Vector2::operator/(const float a) const {
	return Vector2(this->x / a, this->y / a);
}

Vector2& Vector2::operator+=(const Vector2& a) {
	this->x += a.x;
	this->y += a.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& a) {
	this->x -= a.x;
	this->y -= a.y;
	return *this;
}

Vector2& Vector2::operator*=(const float a) {
	this->x *= a;
	this->y *= a;
	return *this;
}

Vector2& Vector2::operator/=(const float a) {
	this->x /= a;
	this->y /= a;
	return *this;
}

float Vector2::Dot(const Vector2& a) const {
	return this->x * a.x + this->y * a.y;
}

float Vector2::Cross(const Vector2& a) const {
	return this->x * a.y - a.x * this->y;
}

float Vector2::Length() const {
	return sqrtf(LengthSq());
}

float Vector2::LengthSq() const {
	return this->x * this->x + this->y * this->y;
}

Vector2& Vector2::Normalize() {
	float length = this->Length();
	if (length != 0) {
		this->x = this->x / length;
		this->y = this->y / length;
	}
	else {
		this->x = 0;
		this->y = 0;
	}
	return *this;
}

Vector2 Vector2::GetNormalize() const {
	float length = this->Length();
	float dx = 0;
	float dy = 0;
	if (length != 0) {
		dx = this->x / length;
		dy = this->y / length;
	}
	return Vector2(dx, dy);
}

Vector2::operator Vector3() const
{
	return Vector3(x, y, 0);
}

Vector2::operator Float4() const
{
	return Float4(x, y, 0, 1);
}
