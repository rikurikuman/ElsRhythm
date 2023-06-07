#include "ColPrimitive3D.h"
#include <math.h>

using namespace ColPrimitive3D;

bool ColPrimitive3D::CheckSphereToSphere(Sphere a, Sphere b)
{
    Vector3 dis = b.pos - a.pos;
    if (dis.LengthSq() <= a.r * a.r + b.r + b.r) {
        return true;
    }
    return false;
}

bool ColPrimitive3D::CheckSphereToPlane(Sphere sphere, Plane plane)
{
    //���_���狅�ւ̋���
    float dist = sphere.pos.Dot(plane.normal);

    //���ʂ̌��_����̋��������������������a��蒷����Γ������ĂȂ�
    if (fabsf(dist - plane.distance) > sphere.r) return false;

    //���Ⴀ�������Ă�
    return true;
}

bool ColPrimitive3D::CheckSphereToAABB(Sphere sphere, AABB box) {
    float minX = box.pos.x - box.size.x;
    float maxX = box.pos.x + box.size.x;
    float minY = box.pos.y - box.size.y;
    float maxY = box.pos.y + box.size.y;
    float minZ = box.pos.z - box.size.z;
    float maxZ = box.pos.z + box.size.z;

    float len = 0; //�ŒZ����

    //X���ɂ���
    if (sphere.pos.x < minX)
        len += (sphere.pos.x - minX) * (sphere.pos.x - minX);
    if (sphere.pos.x > maxX)
        len += (sphere.pos.x - maxX) * (sphere.pos.x - maxX);

    //Y���ɂ���
    if (sphere.pos.y < minY)
        len += (sphere.pos.y - minY) * (sphere.pos.y - minY);
    if (sphere.pos.y > maxY)
        len += (sphere.pos.y - maxY) * (sphere.pos.y - maxY);

    //Z���ɂ���
    if (sphere.pos.z < minZ)
        len += (sphere.pos.z - minZ) * (sphere.pos.z - minZ);
    if (sphere.pos.z > maxZ)
        len += (sphere.pos.z - maxZ) * (sphere.pos.z - maxZ);

    if (len == 0) { //0�Ȃ甠�̒��ɋ��̒��S��������Ă��ƁI
        return true;
    }

    //���̒��S�Ɣ��̍ŒZ���������a��菬�����Ȃ炻��Ⴀ�������Ă�
    if (len < sphere.r * sphere.r) { 
        return true;
    }

    return false;
}

Vector3 GetClosestPoint(Vector3 point, Triangle triangle) {
    //p0�̊O�����H�H�H
    Vector3 p0_p1 = triangle.p1 - triangle.p0;
    Vector3 p0_p2 = triangle.p2 - triangle.p0;
    Vector3 p0_pt = point - triangle.p0;

    float d_01_0p = p0_p1.Dot(p0_pt);
    float d_02_0p = p0_p2.Dot(p0_pt);

    if (d_01_0p < 0 && d_02_0p < 0) {
        return triangle.p0;
    }

    //p1�̊O�����H�H�H
    Vector3 p1_p0 = triangle.p0 - triangle.p1;
    Vector3 p1_p2 = triangle.p2 - triangle.p1;
    Vector3 p1_pt = point - triangle.p1;

    float d_10_1p = p1_p0.Dot(p1_pt);
    float d_12_1p = p1_p2.Dot(p1_pt);

    if (d_10_1p < 0 && d_12_1p < 0) {
        return triangle.p1;
    }

    //p2�̊O�����H�H�H
    Vector3 p2_p0 = triangle.p0 - triangle.p2;
    Vector3 p2_p1 = triangle.p1 - triangle.p2;
    Vector3 p2_pt = point - triangle.p2;

    float d_20_2p = p2_p0.Dot(p2_pt);
    float d_21_2p = p2_p1.Dot(p2_pt);

    if (d_20_2p < 0 && d_21_2p < 0) {
        return triangle.p2;
    }

    float d_01_1p = p0_p1.Dot(p1_pt);
    float d_02_1p = p0_p2.Dot(p1_pt);
    float cc = d_01_0p * d_02_1p - d_01_1p * d_02_0p;

    //p0-p1�ԕӂ̊O�����H�H�H
    if (d_01_0p >= 0 && d_01_1p < 0 && cc <= 0) {
        return triangle.p0 + d_01_0p / (d_01_0p - d_01_1p) * p0_p1;
    }

    float d_01_2p = p0_p1.Dot(p2_pt);
    float d_02_2p = p0_p2.Dot(p2_pt);
    float cb = d_01_2p * d_02_0p - d_01_0p * d_02_2p;

    //p0-p2�ԕӂ̊O�����H�H�H
    if (d_02_0p >= 0 && d_02_2p < 0 && cb <= 0) {
        return triangle.p0 + d_02_0p / (d_02_0p - d_02_2p) * p0_p2;
    }

    float d_12_2p = p1_p2.Dot(p2_pt);
    float d_10_2p = p1_p0.Dot(p2_pt);
    float ca = d_12_1p * d_10_2p - d_12_2p * d_10_1p;

    //p1-p2�ԕӂ̊O�����H�H�H
    if (d_12_1p >= 0 && d_12_2p < 0 && ca <= 0) {
        return triangle.p1 + d_12_1p / (d_12_1p - d_12_2p) * p1_p2;
    }

    float denom = 1.0f / (ca + cb + cc);
    float v = cb * denom;
    float w = cc * denom;
    return triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool ColPrimitive3D::CheckSphereToTriangle(Sphere sphere, Triangle triangle, Vector3* outClosestPos)
{
    Vector3 closest = GetClosestPoint(sphere.pos, triangle);
    if (outClosestPos != nullptr) {
        *outClosestPos = closest;
    }

    Vector3 v = closest - sphere.pos;

    if (v.LengthSq() > sphere.r * sphere.r) return false;

    return true;
}

bool ColPrimitive3D::CheckRayToPlane(Ray ray, Plane plane, float* outDistance, Vector3* outInter)
{
    float d1 = ray.dir.Dot(plane.normal);

    //���ʂ��牓����������ւ̃��C�͂�����񓖂���Ȃ�
    if (d1 > 0) return false;

    float d2 = plane.normal.Dot(ray.start);
    float dist = d2 - plane.distance;

    //�n�_�ƕ��ʂ̋���
    float t = dist / -d1;
    if (t < 0) return false; //0��菬�����Ȃ�n�_�����ɕ��ʂ�����

    if (outDistance) *outDistance = t;
    if (outInter) *outInter = ray.start + t * ray.dir;
    return true;
}

bool ColPrimitive3D::CheckRayToTriangle(Ray ray, Triangle triangle, float* outDistance, Vector3* outInter)
{
    //�O�p�`���܂ޕ���
    Plane plane;
    plane.normal = triangle.normal;
    plane.distance = triangle.normal.Dot(triangle.p0);

    Vector3 interOnPlane;
    //�����������̕��ʂɂ��瓖����Ȃ��Ȃ瓖�����
    if (!CheckRayToPlane(ray, plane, outDistance, &interOnPlane)) return false;

    //��p0_p1�Ԃ�
    Vector3 pt_p0 = triangle.p0 - interOnPlane;
    Vector3 p0_p1 = triangle.p1 - triangle.p0;
    Vector3 m = pt_p0.Cross(p0_p1);
    if (m.Dot(triangle.normal) < 0) return false; //�O���Ȃ瓖�����

    //��p1_p2�Ԃ�
    Vector3 pt_p1 = triangle.p1 - interOnPlane;
    Vector3 p1_p2 = triangle.p2 - triangle.p1;
    m = pt_p1.Cross(p1_p2);
    if (m.Dot(triangle.normal) < 0) return false; //�O���Ȃ瓖�����

    //��p2_p0�Ԃ�
    Vector3 pt_p2 = triangle.p2 - interOnPlane;
    Vector3 p2_p0 = triangle.p0 - triangle.p2;
    m = pt_p2.Cross(p2_p0);
    if (m.Dot(triangle.normal) < 0) return false; //�O���Ȃ瓖�����

    //�������Ă�`�I
    if (outInter) *outInter = interOnPlane;
    return true;
}

bool ColPrimitive3D::CheckRayToSphere(Ray ray, Sphere sphere, float* outDistance, Vector3* outInter)
{
    Vector3 sphereToRay = ray.start - sphere.pos;
    float b = sphereToRay.Dot(ray.dir); //���C�����̕��������Ă��邩����p
    float c = sphereToRay.Dot(sphereToRay) - sphere.r * sphere.r; //���C�̎��_������������p

    if (b > 0.0f && c > 0.0f) return false;

    //���ʎ�
    float d = b * b - c;
    if (d < 0) return false; //�񎟕������̉��Ȃ��A��_���Ȃ�

    float t = -b - sqrtf(d);
    if (t < 0) t = 0; //t�����̏ꍇ�̓��C�̎n�_�����̓����ɂ���(�q�b�g������0�ɂ���)
    if (outDistance) *outDistance = t;
    if (outInter) *outInter = ray.start + t * ray.dir;

    return true;
}