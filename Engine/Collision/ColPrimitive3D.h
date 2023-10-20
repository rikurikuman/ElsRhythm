#pragma once
#include "Vector3.h"

namespace ColPrimitive3D
{
	//球
	struct Sphere {
		Vector3 pos;
		float r;
	};

	//無限平面
	struct Plane {
		Vector3 normal;
		float distance; //原点からの距離
	};

	//ポリゴン
	struct Triangle {
		Vector3 p0;
		Vector3 p1;
		Vector3 p2;
		Vector3 normal;
	};

	//軸平行ボックス
	struct AABB {
		Vector3 pos;
		Vector3 size;
	};

	//レイ
	struct Ray
	{
		Vector3 start;
		Vector3 dir;
	};

	//線分
	struct Segment {
		Vector3 start;
		Vector3 end;
		Vector3 dir;
	};
	
	bool CheckSphereToSphere(Sphere a, Sphere b);
	bool CheckSphereToPlane(Sphere sphere, Plane plane);
	bool CheckSphereToAABB(Sphere sphere, AABB box);
	bool CheckSphereToTriangle(Sphere sphere, Triangle triangle, Vector3* outClosestPos = nullptr);
	bool CheckRayToPlane(Ray ray, Plane plane, float* outDistance = nullptr, Vector3* outInter = nullptr);
	bool CheckRayToTriangle(Ray ray, Triangle triangle, float* outDistance = nullptr, Vector3* outInter = nullptr);
	bool CheckRayToSphere(Ray ray, Sphere sphere, float* outDistance = nullptr, Vector3* outInter = nullptr);
};

