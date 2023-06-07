#pragma once
#include "Colliders.h"
#include "ColPrimitive3D.h"

class SphereCollider : public ICollider {
public:
	ColPrimitive3D::Sphere sphere{};

	SphereCollider() {}
	SphereCollider(Vector3 pos, float r) {
		sphere.pos = pos;
		sphere.r = r;
	}
	virtual std::string GetTypeIndentifier() override {
		return "SphereCollider";
	}

	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) override;
};
