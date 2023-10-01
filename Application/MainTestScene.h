#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"
#include <MultiRenderTest.h>
#include <ArcNoteObj.h>

class MainTestScene : public IScene
{
public:
	MainTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	DebugCamera camera = DebugCamera({0, 0, -5});
	LightGroup light;

	ModelObj skydome;

	ArcNoteObj testObj;

	float timer = 0;
	struct Test {
		ModelObj model;
		Vector3 vec;
		float timer = 0;
	};
	std::list<Test> testList;
};