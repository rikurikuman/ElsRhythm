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
#include <ParticleSprite3D.h>

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

	backGround = Sprite(TextureManager::Load("./Resources/bg.png"), {0.0f, 0.0f});
	backGround.mTransform.position = { 0, 0, -100 };
	backGround.mTransform.UpdateMatrix();

	tempoVeil = Sprite(TextureManager::Load("./Resources/veil.png", "tempoVeil"), {0.0f, 0.0f});
	tempoVeil.mMaterial.mColor = { 0.05f, 0.4f, 1.0f, 0.5f };
	tempoVeil.mTransform.position = { 0, RWindow::GetHeight() / 1.0f, 0};
	tempoVeil.mTransform.scale = { RWindow::GetWidth() / 300.0f, -1.0f, 1.0f };
	tempoVeil.mTransform.UpdateMatrix();

	camera.mViewProjection.mEye = { 0, 11.0f, -12.0f };
	camera.mAngle = { 90, -25 };
	camera.mFreeFlag = true;
	camera.mViewProjection.UpdateMatrix();

	TextureManager::Load("./Resources/ptCircle.png", "ParticleCircle");
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

	veilTimer += TimeManager::deltaMiliTime;
	float bpm = gameController.music.GetBPM(gameController.music.ConvertMiliSecondsToBeat(gameController.time));
	float whole = gameController.music.GetWholeNoteLength(gameController.music.ConvertMiliSecondsToBeat(gameController.time));
	tempoVeil.mMaterial.mColor.a = 0.01f + 0.1f * (min(1, Util::GetRatio(0, whole / 4, veilTimer)) - max(0, Util::GetRatio(0, whole / 4, veilTimer) - 1));
	if (veilTimer >= whole / 2) {
		veilTimer -= whole / 2;
	}

	shotTimer += TimeManager::deltaTime;

	if (shotTimer >= 0.15f) {
		if (shotCount % 2 == 0) {
			ParticleSprite3D::Spawn({ -20, 0, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 1, 0, 1, 1 }, { 0, 0, -1 }, 150, 150, 0.5f, 0.5f, 5.0f);
			ParticleSprite3D::Spawn({ 20, 0, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 1, 0, 1, 1 }, { 0, 0, -1 }, 150, 150, 0.5f, 0.5f, 5.0f);
			}
		else {
			ParticleSprite3D::Spawn({ -12, 6, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 0, 1, 0, 1 }, { 0, 0, -1 }, 150, 150, 0.5f, 0.5f, 5.0f);
			ParticleSprite3D::Spawn({ 12, 6, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 0, 1, 0, 1 }, { 0, 0, -1 }, 150, 150, 0.5f, 0.5f, 5.0f);
		}

		ParticleSprite3D::Spawn({ 25, 3, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 0, 1, 1, 1 }, { 0, 0, -1 }, 150, 150, 0.5f, 0.5f, 5.0f);
		ParticleSprite3D::Spawn({ -25, 3, 80 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 0, 1, 1, 1 }, { 0, 0, -1 }, 150, 150, 0.5f, 0.5f, 5.0f);

		shotTimer = 0;
		shotCount++;
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

	backGround.TransferBuffer();
	backGround.Draw();
	tempoVeil.TransferBuffer();
	tempoVeil.Draw();
}