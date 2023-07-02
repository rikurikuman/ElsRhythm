#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Sprite.h"

class ControllerScene : public IScene
{
public:
	ControllerScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;
	ModelObj model;

	Camera camera = Camera();
	LightGroup light;

	float rot[3] = { 0, 0, 0 };
	float scale[3] = { 1, 1, 1 };
};