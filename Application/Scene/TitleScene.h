/*
* @file TitleScene.h
* @brief タイトル画面のシーン
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"

class TitleScene : public IScene
{
public:
	TitleScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Camera camera = Camera();
	LightGroup light;

	Sprite bgHexa1;
	Sprite bgHexa2;
};
