#include "RayCollider.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"

void RayCollider::Update()
{
    skip = false;
    if (!pierce) {
        memDis = FLT_MAX;
        nearest = nullptr;
        Colliders::ColListAccessor access;
        for (std::unique_ptr<ICollider>& ptr : *access.list) {
            CollisionInfo info;
            if (Collide(ptr.get(), &info)) {
                if (info.hasDistance && info.hasInter) {
                    if (info.distance < memDis) {
                        memDis = info.distance;
                        memInfo = info;
                        nearest = ptr.get();
                    }
                }
            }
        }
        skip = true;
    }
}

bool RayCollider::Collide(ICollider* hit, CollisionInfo* outputInfo)
{
    if (skip) {
        if (outputInfo) *outputInfo = memInfo;
        return hit == nearest;
    }

    CollisionInfo info;
    if (hit->GetTypeIndentifier() == "SphereCollider") {
        SphereCollider* hitT = static_cast<SphereCollider*>(hit);

        if (ColPrimitive3D::CheckRayToSphere(this->ray, hitT->sphere, &info.distance, &info.inter)) {
            info.hitCollider = hit;
            info.hasDistance = true;
            info.hasInter = true;
            if (outputInfo) {
                *outputInfo = info;
            }
            return true;
        }
        return false;
    }

    if (hit->GetTypeIndentifier() == "PolygonCollider") {
        PolygonCollider* hitT = static_cast<PolygonCollider*>(hit);

        if (ColPrimitive3D::CheckRayToTriangle(this->ray, hitT->polygon, &info.distance, &info.inter)) {
            info.hitCollider = hit;
            info.hasDistance = true;
            info.hasInter = true;
            if (outputInfo) {
                *outputInfo = info;
            }
            return true;
        }
        return false;
    }

    return false;
}
