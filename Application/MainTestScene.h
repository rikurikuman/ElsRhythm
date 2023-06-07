#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "PostEffect.h"
#include "SRConstBuffer.h"

class MainTestScene : public IScene
{
public:
	MainTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	struct TestObj {
	public:
		ModelObj obj;
		Vector3 speed;
		float timer;
	};

	DebugCamera camera = DebugCamera({0, 0, -5});
	LightGroup light;

	ModelObj sphere;
	ModelObj sphere2;
	Sprite sprite;
	Sprite sprite2;

	std::list<TestObj> testObjList;

	PostEffect hoge;

	float timer = 0;
};