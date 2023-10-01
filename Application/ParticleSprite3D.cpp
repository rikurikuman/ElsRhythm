#include "ParticleSprite3D.h"
#include "TimeManager.h"
#include "Util.h"
#include <Camera.h>
#include <Renderer.h>
#include <SimpleDrawer.h>

using namespace Util;


ParticleSprite3D::ParticleSprite3D(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, Vector3 vec, float startSpeed, float endSpeed, float startSize, float endSize, float time)
{
	this->sprite.SetTexture(texHandle);
	this->sprite.SetTexRect(texRect.left, texRect.top, texRect.right - texRect.left, texRect.bottom - texRect.top);
	this->sprite.SetAnchor(texAnchor);
	this->sprite.mBlendMode = Sprite::BlendMode::Add;
	this->pos = pos;
	this->color = color;
	this->vec = vec;
	this->startSpeed = startSpeed;
	this->endSpeed = endSpeed;
	this->startSize = startSize;
	this->endSize = endSize;
	this->maxTime = time;
}

void ParticleSprite3D::Update()
{
	timer += TimeManager::deltaTime;
	float t = timer / maxTime;
	if (t > 1) t = 1;

	if (t >= 1) {
		isDeleted = true;
		return;
	}

	float fx = 1 - powf(1 - t, 3);
	speed = startSpeed * (1 - fx) + endSpeed * fx;
	size = startSize * (1 - fx) + endSize * fx;
	sprite.mMaterial.mColor = color;
	sprite.mMaterial.mColor.a = color.a * (1 - fx);

	pos += vec * speed * TimeManager::deltaTime;
}

void ParticleSprite3D::Draw()
{
	Vector2 screenPos = Camera::sNowCamera->mViewProjection.WorldToScreen({ pos.x, pos.y, pos.z });
	
	sprite.mTransform.position = screenPos;
	sprite.mTransform.position.z = -1;
	sprite.mTransform.scale = { size, size, 1 };
	sprite.mTransform.UpdateMatrix();
	
	sprite.TransferBuffer();
	sprite.Draw();
}

void ParticleSprite3D::Spawn(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, Vector3 vec, float startSpeed, float endSpeed, float startSize, float endSize, float time) {
	manageParticleObjList.emplace_back(std::make_unique<ParticleSprite3D>(pos, texHandle, texRect, texAnchor, color, vec, startSpeed, endSpeed, startSize, endSize, time));
}