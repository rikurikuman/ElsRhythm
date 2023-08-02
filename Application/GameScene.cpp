#include "GameScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>
#include <ParticleObject.h>
#include <SceneManager.h>
#include <TitleScene.h>
#include <SimpleSceneTransition.h>

std::string GameScene::sChartName;

GameScene::GameScene()
{
	judgeLine = ModelObj("Cube");
	judgeLine.mTransform.position = { 0, -0.04f, 0 };
	judgeLine.mTransform.scale = { 16, 0.1f, 0.2f };
	judgeLine.mTransform.UpdateMatrix();
	judgeLine.mTuneMaterial.mColor = { 0.3f, 0, 0.3f, 1.0f };

	lane = ModelObj("Cube");
	lane.mTransform.position = { 0, -0.1f, 0 };
	lane.mTransform.scale = { 16, 0.1f, 160.0f };
	lane.mTransform.UpdateMatrix();
	lane.mTuneMaterial.mColor = { 0.2f, 0.2f, 0.2f, 1.0f };

	for (int32_t i = 0; i < 3; i++) {
		lines[i] = ModelObj("Cube");
		lines[i].mTransform.position = { -4.0f + 4.0f * i, -0.045f, 0 };
		lines[i].mTransform.scale = { 0.2f, 0.1f, 160.0f };
		lines[i].mTransform.UpdateMatrix();
		lines[i].mTuneMaterial.mColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	for (int32_t i = 0; i < 4; i++) {
		inputlines[i] = ModelObj("Cube");
		inputlines[i].mTransform.position = { -6.0f + 4.0f * i, -0.05f, 0 };
		inputlines[i].mTransform.scale = { 4.0f, 0.1f, 160.0f };
		inputlines[i].mTransform.UpdateMatrix();
		inputlines[i].mTuneMaterial.mColor = { 0.4f, 0.4f, 0.4f, 0.2f };
	}

	camera.mViewProjection.mEye = { 0, 11.0f, -12.0f };
	camera.mAngle = { 90, -25 };
	camera.mFreeFlag = true;
	camera.mViewProjection.UpdateMatrix();

	TextureManager::Load("./Resources/judgeText.png", "judgeText");

	RAudio::Load("Resources/Sound/Judge_Perfect.wav", "JudgePerfect");
	RAudio::Load("Resources/Sound/Judge_Hit.wav", "JudgeHit");
	RAudio::Load("Resources/Sound/Judge_Miss.wav", "JudgeMiss");
	RAudio::Load("Resources/Sound/VerticalSlash.wav", "VerticalSlash");

	chartFile = ChartFile("Charts/" + sChartName);
	chartFile.Load();
	gameController.chart = chartFile;
	gameController.Load();
	gameController.Init();
}

void GameScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	ParticleObject::Clear();
	gameController.time = -6000;
	gameController.playing = true;
}

void GameScene::Update()
{
	light.Update();
	camera.Update();
	gameController.Update();
	ParticleObject::ManageParticleObject(true);

	if (RInput::GetKey(DIK_ESCAPE)) {
		gameController.playing = false;
		if(!SceneManager::IsSceneChanging()) SceneManager::Change<TitleScene, SimpleSceneTransition>();
	}
}

void GameScene::Draw()
{
	judgeLine.TransferBuffer(Camera::sNowCamera->mViewProjection);
	judgeLine.Draw();
	lane.TransferBuffer(Camera::sNowCamera->mViewProjection);
	lane.Draw();
	for (int32_t i = 0; i < 3; i++) {
		lines[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
		lines[i].Draw();
	}

	std::array<int32_t, 4> laneKeys = { DIK_F, DIK_G, DIK_H, DIK_J };
	for (int32_t i = 0; i < 4; i++) {
		if (RInput::GetKey(laneKeys[i])
			|| (i == 2 && RInput::GetMouseClick(0))
			|| (i == 3 && RInput::GetMouseClick(1))) {
			inputlines[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
			inputlines[i].Draw();
		}
	}

	SimpleDrawer::DrawString(0, 0, 0, Util::StringFormat("%.2f, %.2f, %.2f, %.2f, %.2f", camera.mViewProjection.mEye.x, camera.mViewProjection.mEye.y, camera.mViewProjection.mEye.z, camera.mAngle.x, camera.mAngle.y));
}