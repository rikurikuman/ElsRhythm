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

	struct Button {
		Vector2 pos;
		Vector2 size;
		Color color;

		bool isHover;
		bool isClick;

		void Update();
		void Draw();
	};

	Button goEasyButton{};
	Button goNormalButton{};
	Button goHardButton{};
	Button goTutorialButton{};
};