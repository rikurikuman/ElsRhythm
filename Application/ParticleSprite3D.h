#pragma once
#include "ParticleObject.h"
#include <Sprite.h>
#include <Rect.h>

class ParticleSprite3D : public ParticleObject
{
public:
	Sprite sprite;
	Color color;
	Vector3 vec;
	float speed = 0;
	float startSpeed;
	float endSpeed;
	float size = 0;
	float startSize;
	float endSize;
	float maxTime;
	float timer = 0;

	ParticleSprite3D(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, Vector3 vec, float startSpeed, float endSpeed, float startSize, float endSize, float time);

	void Update() override;
	void Draw() override;

	static void Spawn(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, Vector3 vec, float startSpeed, float endSpeed, float startSize, float endSize, float time);
};