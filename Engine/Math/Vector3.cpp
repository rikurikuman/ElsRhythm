#include "Vector3.h"
#include <math.h>
#include "Vector2.h"
#include "Float4.h"

const float EPSILON = 0.000001f;

Vector3::Vector3() : x(0), y(0), z(0) {}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3::~Vector3() {}

bool Vector3::operator==(const Vector3& a) const {
	return this->x == a.x && this->y == a.y && this->z == a.z;
}

bool Vector3::operator!=(const Vector3& a) const {
	return this->x != a.x || this->y != a.y || this->z == a.z;
}

bool Vector3::operator<(const Vector3& a) const
{
	if (this->x < a.x) return true;
	if (this->x == a.x) {
		if (this->y < a.y) return true;
		if (this->y == a.y) {
			if (this->z < a.z) return true;
		}
	}
	return false;
}

Vector3 Vector3::operator+(const Vector3& a) const {
	return Vector3(this->x + a.x, this->y + a.y, this->z + a.z);
}

Vector3 Vector3::operator-(const Vector3& a) const {
	return Vector3(this->x - a.x, this->y - a.y, this->z - a.z);
}

Vector3 Vector3::operator-() const {
	return Vector3(-this->x, -this->y, -this->z);
}

Vector3 Vector3::operator*(const float a) const {
	return Vector3(this->x * a, this->y * a, this->z * a);
}

Vector3 operator*(const float a, const Vector3& b) {
	return Vector3(b.x * a, b.y * a, b.z * a);
}

Vector3 Vector3::operator/(const float a) const {
	return Vector3(this->x / a, this->y / a, this->z / a);
}

Vector3& Vector3::operator+=(const Vector3& a) {
	this->x += a.x;
	this->y += a.y;
	this->z += a.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& a) {
	this->x -= a.x;
	this->y -= a.y;
	this->z -= a.z;
	return *this;
}

Vector3& Vector3::operator*=(const Vector3& a)
{
	this->x *= a.x;
	this->y *= a.y;
	this->z *= a.z;
	return *this;
}

Vector3& Vector3::operator*=(const float a) {
	this->x *= a;
	this->y *= a;
	this->z *= a;
	return *this;
}

Vector3& Vector3::operator/=(const float a) {
	this->x /= a;
	this->y /= a;
	this->z /= a;
	return *this;
}

float Vector3::Dot(const Vector3& a) const {
	return this->x * a.x + this->y * a.y + this->z * a.z;
}

Vector3 Vector3::Cross(const Vector3& a) const {
	float x = this->y * a.z - this->z * a.y;
	float y = this->z * a.x - this->x * a.z;
	float z = this->x * a.y - this->y * a.x;
	return Vector3(x, y, z);
}

float Vector3::Length() const {
	return sqrtf(LengthSq());
}

float Vector3::LengthSq() const {
	return this->x * this->x + this->y * this->y + this->z * this->z;
}

Vector3& Vector3::Normalize() {
	float length = this->Length();
	if (length != 0) {
		this->x = this->x / length;
		this->y = this->y / length;
		this->z = this->z / length;
	}
	else {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
	return *this;
}

Vector3 Vector3::GetNormalize() const {
	float length = this->Length();
	float dx = 0;
	float dy = 0;
	float dz = 0;
	if (length != 0) {
		dx = this->x / length;
		dy = this->y / length;
		dz = this->z / length;
	}
	return Vector3(dx, dy, dz);
}

Vector3::operator Vector2() const
{
	return Vector2(x, y);
}

Vector3::operator Float4() const
{
	return Float4(x, y, z, 1);
}

const Vector3 lerp(const Vector3& start, const Vector3& end, const float t)
{
	return start + (end - start) * t;
}
