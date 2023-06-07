#pragma once
#include "Colliders.h"
#include "ColPrimitive3D.h"

class RayCollider : public ICollider {
public:
	ColPrimitive3D::Ray ray{};
	bool pierce = false;

	RayCollider() {}
	RayCollider(Vector3 start, Vector3 dir) {
		ray.start = start;
		ray.dir = dir;
	}
	RayCollider(Vector3 start, Vector3 dir, bool pierce) {
		ray.start = start;
		ray.dir = dir;
		this->pierce = pierce;
	}
	virtual std::string GetTypeIndentifier() override {
		return "RayCollider";
	}

	virtual void Update() override;
	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) override;

private:
	bool skip = false;
	float memDis = 0;
	CollisionInfo memInfo;
	ICollider* nearest = nullptr;
};
