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
#include <ResultScene.h>
#include <BackGroundSelector.h>
#include <codecvt>

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

	tempoVeil = Sprite(TextureManager::Load("./Resources/veil.png", "tempoVeil"), {0.0f, 0.0f});
	tempoVeil.mMaterial.mColor = { 0.05f, 0.4f, 1.0f, 0.5f };
	tempoVeil.mTransform.position = { 0, RWindow::GetHeight() / 1.0f, 0};
	tempoVeil.mTransform.scale = { RWindow::GetWidth() / 300.0f, -1.0f, 1.0f };
	tempoVeil.mTransform.UpdateMatrix();

	tutorialText.mTransform.position = { RWindow::GetWidth() / 2.0f, 80, 5 };
	tutorialText.mTransform.UpdateMatrix();

	tutorialBack = Sprite(TextureManager::Load("./Resources/veil.png", "tempoVeil"), { 0.0f, 0.0f });
	tutorialBack.mMaterial.mColor = { 0.0f, 0.0f, 0.0f, 0.5f };
	tutorialBack.mTransform.position = { 0, 0, 0 };
	tutorialBack.mTransform.scale = { RWindow::GetWidth() / 300.0f, 1.0f, 1.0f };
	tutorialBack.mTransform.UpdateMatrix();

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
	BackGroundSelector::Select(chartFile.bgName);
}

void GameScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	ParticleObject::Clear();
	gameController.time = -6000;
	gameController.playing = true;
}

void GameScene::Finalize()
{
	ParticleObject::Clear();
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

	if(gameController.finished) finishTimer += TimeManager::deltaTime;
	if (finishTimer >= 2.0f) {
		ResultScene::sScore = gameController.score;
		ResultScene::sCountPerfect = gameController.countJudgePerfect;
		ResultScene::sCountHit = gameController.countJudgeHit;
		ResultScene::sCountMiss = gameController.countJudgeMiss;
		if (!SceneManager::IsSceneChanging()) SceneManager::Change<ResultScene, SimpleSceneTransition>();
	}

	BackGroundSelector::Update();

	for (Event& e : EventSystem::GetTriggeredEvents("tutorial")) {
		std::wstring wStr = Util::ConvertStringToWString(e.paramString, CP_UTF8);

		tutorialText.SetTexture(TextDrawer::CreateStringTexture(wStr, "", 32, "tutorialtext"));
		tutorialText.SetAnchor({ 0.5f, 0.5f });
		tutorialAliveTime = e.paramFloat;
	}

	if (tutorialAliveTime != 0) {
		tutorialTimer += TimeManager::deltaTime;

		if ((tutorialAliveTime - tutorialTimer) < 1.0f) {
			float t = max(0.0f, min(1.0f, (tutorialAliveTime - tutorialTimer) / 1.0f));

			float fx = 1 - powf(1 - t, 4);

			tutorialText.mMaterial.mColor = { 1, 1, 1, 1.0f * fx };
			tutorialBack.mMaterial.mColor = { 0, 0, 0, 2.0f * fx };
		}
		else {
			float t = min(1.0f,  tutorialTimer / 1.0f);

			float fx = 1 - powf(1 - t, 4);

			tutorialText.mMaterial.mColor = { 1, 1, 1, 1.0f * fx };
			tutorialBack.mMaterial.mColor = { 0, 0, 0, 2.0f * fx };
		}

		if (tutorialTimer >= tutorialAliveTime) {
			tutorialTimer = 0;
			tutorialAliveTime = 0;
		}
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

	tempoVeil.TransferBuffer();
	tempoVeil.Draw();

	BackGroundSelector::Draw();

	if (tutorialAliveTime != 0) {
		tutorialText.TransferBuffer();
		tutorialBack.TransferBuffer();
		tutorialText.Draw();
		tutorialBack.Draw();
	}
}