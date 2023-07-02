#pragma once
#include "IScene.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"
#include "ModelObj.h"
#include <Camera.h>
#include <LightGroup.h>
#include "RayCollider.h"

class CollidersScene : public IScene {
public:
	CollidersScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Camera camera;
	LightGroup light;

	Collider<SphereCollider> colSphere;
	Collider<SphereCollider> colSphere2;
	Collider<PolygonCollider> colPolygon;
	Collider<RayCollider> colRay;
	ModelObj sphere;
	ModelObj sphere2;
	ModelObj ray;

	float radiusA = 1;
	float radiusB = 1;

	bool pierce = false;
	float memDis = 0;

	bool autoMoveA = false;
	bool autoMoveB = true;
	bool autoMoveC = true;
	float moveDirA = 1;
	float moveDirB = 1;
	float moveDirC = -1;

	Vector3 posA = { 0, 0, 1 };
	Vector3 posB = { -1, 0, -1 };
	Vector3	posC = { 1, 0, -1 };

	GraphicsPipeline polygonPipeline;
	VertexBuffer vertBuff;
	RConstBuffer<TransformBuffer> transformBuff;
	RConstBuffer<MaterialBuffer> materialBuff;
	RConstBuffer<ViewProjectionBuffer> viewProjectionBuff;
};