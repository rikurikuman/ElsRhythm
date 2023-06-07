#pragma once
#include "Colliders.h"
#include "ColPrimitive3D.h"

class PolygonCollider : public ICollider {
public:
	//ポリゴン情報、各頂点は時計回りで指定されていること
	ColPrimitive3D::Triangle polygon{};

	PolygonCollider() {}
	PolygonCollider(Vector3 posA, Vector3 posB, Vector3 posC) {
		polygon.p0 = posA;
		polygon.p1 = posB;
		polygon.p2 = posC;

		Vector3 v1 = posB - posA;
		Vector3 v2 = posC - posB;

		Vector3 normal = v1.Cross(v2);
		normal.Normalize();
		polygon.normal = normal;
	}
	PolygonCollider(Vector3 posA, Vector3 posB, Vector3 posC, Vector3 normal) {
		polygon.p0 = posA;
		polygon.p1 = posB;
		polygon.p2 = posC;
		polygon.normal = normal;
	}

	virtual std::string GetTypeIndentifier() override {
		return "PolygonCollider";
	}

	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) override;
};
