#include "BGSimple.h"
#include <TimeManager.h>
#include <ParticleSprite3D.h>
#include <EventSystem.h>
#include <Camera.h>

BGSimple::BGSimple()
{
	backGround = Sprite(TextureManager::Load("./Resources/bg.png"), { 0.0f, 0.0f });
	backGround.mTransform.position = { 0, 0, -100 };
	backGround.mTransform.UpdateMatrix();
}

void BGSimple::Update()
{
	shotTimer += TimeManager::deltaTime;

	if (shotTimer >= 0.15f) {
		if (shotCount % 2 == 0) {
			ParticleSprite3D::Spawn({ -20, 0, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 1, 0.1f, 1, 1 }, 0, 0, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0 }, { 10, 10 }, { 0, 0 }, 0.5f, 5.0f, 5.0f);
			ParticleSprite3D::Spawn({ 20, 0, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 1, 0.1f, 1, 1 }, 0, 0, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0 }, { 10, 10 }, { 0, 0 }, 0.5f, 5.0f, 5.0f);
		}
		else {
			ParticleSprite3D::Spawn({ -12, 6, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 0.1f, 1, 0.1f, 1 }, 0, 0, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0 }, { 10, 10 }, { 0, 0 }, 0.5f, 5.0f, 5.0f);
			ParticleSprite3D::Spawn({ 12, 6, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 0.1f, 1, 0.1f, 1 }, 0, 0, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0 }, { 10, 10 }, { 0, 0 }, 0.5f, 5.0f, 5.0f);
		}

		ParticleSprite3D::Spawn({ 25, 3, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 0.1f, 1, 1, 1 }, 0, 0, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0 }, { 10, 10 }, { 0, 0 }, 0.5f, 5.0f, 5.0f);
		ParticleSprite3D::Spawn({ -25, 3, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 0.1f, 1, 1, 1 }, 0, 0, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0 }, { 10, 10 }, { 0, 0 }, 0.5f, 5.0f, 5.0f);

		if (shotCount % 10 == 0) {
			ParticleSprite3D::Spawn({ 0, 0, 100 }, TextureManager::Load("./Resources/ptHexagon.png", "ParticleHexagon"), RRect(0, 512, 0, 512), { 0.5f, 0.5f }, { 0.1f, 1, 1, 1 }, Util::GetRand(0.0f, 360.0f), 240, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0, -150 }, { 0, 0 }, { 50, 50 }, { 0, 0 }, 0.5f, 5.0f, 5.0f);
		}


		shotTimer = 0;
		shotCount++;
	}

	for (int32_t i = 0; i < 10; i++) {
		float speed = Util::GetRand(-100.0f, -200.0f);
		float size = Util::GetRand(5.0f, 10.0f);
		ParticleSprite3D::Spawn({ Util::GetRand(-25.0f, 25.0f), Util::GetRand(-8.0f, 8.0f), 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { Util::GetRand(0.0f, 1.0f), Util::GetRand(0.0f, 1.0f), Util::GetRand(0.0f, 1.0f), 1.0f }, 0, 0, { 0, 0, speed }, { 0, 0, speed }, { 0, 0, speed }, { 0, 0 }, { size, size }, { 0, 0 }, 0.5f, 5.0f, 5.0f);
	}
}

void BGSimple::Draw()
{
	backGround.TransferBuffer();
	backGround.Draw();
}
