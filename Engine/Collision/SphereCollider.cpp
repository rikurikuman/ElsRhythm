#include "SphereCollider.h"
#include "PolygonCollider.h"

bool SphereCollider::Collide(ICollider* hit, CollisionInfo* outputInfo)
{
    CollisionInfo info;
    if (hit->GetTypeIndentifier() == "SphereCollider") {
        SphereCollider* hitT = static_cast<SphereCollider*>(hit);
        
        if (ColPrimitive3D::CheckSphereToSphere(this->sphere, hitT->sphere)) {
            if (outputInfo) {
                outputInfo->hitCollider = hit;
            }
            return true;
        }
        return false;
    }

    if (hit->GetTypeIndentifier() == "PolygonCollider") {
        PolygonCollider* hitT = static_cast<PolygonCollider*>(hit);

        if (ColPrimitive3D::CheckSphereToTriangle(this->sphere, hitT->polygon, &info.closestPos)) {
            info.hitCollider = hit;
            info.hasClosestPos = true;
            if (outputInfo) {
                *outputInfo = info;
            }
            return true;
        }
        return false;
    }

    return false;
}
