#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"

class ResultScene : public IScene
{
public:
	ResultScene();

	void Init() override;
	void Update() override;
	void Draw() override;

	static int32_t sScore;
	static int32_t sCountPerfect;
	static int32_t sCountHit;
	static int32_t sCountMiss;

private:
	Camera camera = Camera();
	LightGroup light;

	int32_t step = 0;
	float timer = 0;

	Sprite rankTextA;
	Sprite rankTextB;
	Sprite scoreTextA;
	Sprite scoreTextB;
	Sprite countPerfectText;
	Sprite countHitText;
	Sprite countMissText;

	struct Button {
		Vector2 pos;
		Vector2 size;
		Color color;

		bool isHover;
		bool isClick;

		void Update();
		void Draw();
	};

	Button goRetryButton{};
	Button goTitleButton{};
};