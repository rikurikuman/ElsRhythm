#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"
#include <ChartFile.h>
#include <RhythmGameController.h>

class GameScene : public IScene
{
public:
	GameScene();

	void Init() override;
	void Update() override;
	void Draw() override;

	static std::string sChartName;

private:
	DebugCamera camera = DebugCamera({0, 0, -5});
	LightGroup light;

	ModelObj judgeLine;
	ModelObj lane;
	std::array<ModelObj, 3> lines;
	std::array<ModelObj, 4> inputlines;

	Sprite backGround;

	Sprite tempoVeil;
	float veilTimer = 0;

	float shotTimer = 0;
	int32_t shotCount = 0;

	ChartFile chartFile;
	RhythmGameController gameController;
};