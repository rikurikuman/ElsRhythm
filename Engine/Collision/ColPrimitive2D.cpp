#include "ColPrimitive2D.h"

using namespace std;
using namespace ColPrimitive2D;

array<Vector2, 4> ColPrimitive2D::Rect::ToPointArray()
{
    array<Vector2, 4> arr = {};

    float pLeft = p.x - w / 2;
    float pRight = p.x + w / 2;
    float pTop = p.y + h / 2;
    float pBottom = p.y - h / 2;

    arr[0] = { pLeft, pTop }; //ç∂è„
    arr[1] = { pRight, pTop }; //âEè„
    arr[2] = { pLeft, pBottom }; //ç∂â∫
    arr[3] = { pRight, pBottom } ; //âEâ∫

    return arr;
}

std::array<Segment, 4> ColPrimitive2D::Rect::ToSideArray()
{
    std::array<Segment, 4> arr = {};

    float pLeft = p.x - w / 2;
    float pRight = p.x + w / 2;
    float pTop = p.y + h / 2;
    float pBottom = p.y - h / 2;

    arr[0] = { {pLeft, pTop}, {pLeft, pBottom} }; //ç∂
    arr[1] = { {pRight, pTop}, {pRight, pBottom} }; //âE
    arr[2] = { {pLeft, pTop}, {pRight, pTop} }; //è„
    arr[3] = { {pLeft, pBottom}, {pRight, pBottom} }; //â∫

    return arr;
}

bool ColPrimitive2D::IsHit(Rect a, Rect b)
{
    float xDiff = a.p.x - b.p.x;
    float yDiff = a.p.y - b.p.y;
    float wSum = a.w / 2 + b.w / 2;
    float hSum = a.h / 2 + b.h / 2;

    return abs(xDiff) <= wSum && abs(yDiff) <= hSum;
}

bool ColPrimitive2D::IsHit(Segment seg, Rect rect)
{
    for (Segment rectseg : rect.ToSideArray()) {
        Vector2 v = rectseg.p - seg.p;
        float t = v.Cross(seg.v.GetNormalize()) / seg.v.GetNormalize().Cross(rectseg.v);
        if (t >= 0 && t <= 1) {
            Vector2 p = rectseg.p + t * rectseg.v;
            if ((p - seg.p).LengthSq() == 0 || ((p - seg.p).GetNormalize().Dot(seg.v.GetNormalize()) > 0 && (p - seg.p).LengthSq() <= seg.v.LengthSq())) {
                return true;
            }
        }
    }

    return false;
}

ColResult ColPrimitive2D::CheckHit(Segment seg, Rect rect)
{
    ColResult result;

    array<Segment, 4> arr = rect.ToSideArray();
    float nearestHitDistance = 0;

    for (int32_t i = 0; i < 4; i++) {
        Segment rectseg = arr[i];
        Vector2 v = rectseg.p - seg.p;
        Vector2 v2 = seg.p - rectseg.p;
        float t = v.Cross(seg.v.GetNormalize()) / seg.v.GetNormalize().Cross(rectseg.v);
        float t2 = v2.Cross(rectseg.v.GetNormalize()) / rectseg.v.GetNormalize().Cross(seg.v);
        if ((t >= 0 && t <= 1) && (t2 >= 0 && t2 <= 1)) {
            Vector2 p = rectseg.p + t * rectseg.v;
            Vector2 p2 = seg.p + t2 * seg.v;
            if (true) {
                if (!result.hit) {
                    result.hit = true;
                    result.hitPos = p;
                    result.hasHitPos = true;
                    result.hitSide = i;
                    result.hitSideBit.set(i);
                    nearestHitDistance = (p - seg.p).LengthSq();
                }
                else {
                    float hitDistance = (p - seg.p).LengthSq();
                    if (hitDistance < nearestHitDistance) {
                        result.hitPos = p;
                        result.hitSide = i;
                        result.hitSideBit = 0;
                        result.hitSideBit.set(i);
                        nearestHitDistance = hitDistance;
                    }
                    else if (hitDistance == nearestHitDistance) {
                        result.hitPos = p;
                        result.hitSide = i;
                        result.hitSideBit.set(i);
                    }
                }
            }
        }
    }

    return result;
}
