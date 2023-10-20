#pragma once
#include "IParticle.h"
#include <Image3D.h>
#include <Rect.h>

class ParticleImage3D : public IParticle
{
private:
	Image3D image;

	Vector3 pos;
	Color color;
	Vector3 velocity;
	Vector3 inVelocity;
	Vector3 stayVelocity;
	Vector3 outVelocity;
	float radian = 0;
	float rotSpeed = 0;
	Vector2 size;
	Vector2 inSize;
	Vector2 staySize;
	Vector2 outSize;
	float inTime;
	float stayTime;
	float outTime;
	float maxTime;
	float timer = 0;

public:
	ParticleImage3D(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float angle, float rotSpeed, Vector3 inVelocity, Vector3 stayVelocity, Vector3 outVelocity, Vector2 inSize, Vector2 staySize, Vector2 outSize, float inTime, float stayTime, float outTime);

	void Update() override;
	void Draw() override;

	//std::function<void()> GetBatchClearFunc() override;
	//std::function<void()> GetBatchDrawFunc() override;

	static void Spawn(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float angle, float rotSpeed, Vector3 inVelocity, Vector3 stayVelocity, Vector3 outVelocity, Vector2 inSize, Vector2 staySize, Vector2 outSize, float inTime, float stayTime, float outTime);

private:
	struct ParticleInfo {

	};
	//static std::list<

	//static void ClearBatch();
	//static void DrawBatch();
};
