#include "ResultScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>
#include <SceneManager.h>
#include <GameScene.h>
#include <SimpleSceneTransition.h>
#include <ParticleObject.h>
#include <TitleScene.h>

int32_t ResultScene::sScore = 0;
int32_t ResultScene::sCountPerfect = 0;
int32_t ResultScene::sCountHit = 0;
int32_t ResultScene::sCountMiss = 0;

ResultScene::ResultScene()
{
	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	rankTextA = Sprite(TextDrawer::CreateStringTexture("Rank", "", 48, "RankTextA"));

	if (sScore >= 9900000) {
		rankTextB = Sprite(TextDrawer::CreateStringTexture("S", "", 256, "RankTextB"));
	}
	else if (sScore >= 9800000) {
		rankTextB = Sprite(TextDrawer::CreateStringTexture("A", "", 256, "RankTextB"));
	}
	else if (sScore >= 9500000) {
		rankTextB = Sprite(TextDrawer::CreateStringTexture("B", "", 256, "RankTextB"));
	}
	else if (sScore >= 9200000) {
		rankTextB = Sprite(TextDrawer::CreateStringTexture("C", "", 256, "RankTextB"));
	}
	else if (sScore >= 8900000) {
		rankTextB = Sprite(TextDrawer::CreateStringTexture("D", "", 256, "RankTextB"));
	}
	else if (sScore >= 8600000) {
		rankTextB = Sprite(TextDrawer::CreateStringTexture("E", "", 256, "RankTextB"));
	}
	else {
		rankTextB = Sprite(TextDrawer::CreateStringTexture("F", "", 256, "RankTextB"));
	}
	rankTextB.mMaterial.mColor = { 1, 0.7f, 0.3f, 1 };

	scoreTextA = Sprite(TextDrawer::CreateStringTexture("Score", "", 32, "ScoreTextA"));
	scoreTextB = Sprite(TextDrawer::CreateStringTexture(Util::StringFormat("%08d", sScore), "", 60, "ScoreTextB"));
	countPerfectText = Sprite(TextDrawer::CreateStringTexture(Util::StringFormat("Perfect: %d", sCountPerfect), "", 25, "cPerfectText"));
	countHitText = Sprite(TextDrawer::CreateStringTexture(Util::StringFormat("Hit: %d", sCountHit), "", 25, "cHitText"));
	countMissText = Sprite(TextDrawer::CreateStringTexture(Util::StringFormat("Hoge: %d", sCountMiss), "", 25, "cMissText"));

	scoreTextB.mMaterial.mColor = { 0.1f, 0.7f, 0.5f, 1 };

	goRetryButton.pos = Vector2(440, 620);
	goRetryButton.size = Vector2(100, 50);
	goRetryButton.color = { 1, 1, 1, 1 };

	goTitleButton.pos = Vector2(840, 620);
	goTitleButton.size = Vector2(100, 50);
	goTitleButton.color = { 1, 1, 1, 1 };
}

void ResultScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
	ParticleObject::Clear();
}

void ResultScene::Update()
{
	light.Update();
	camera.Update();

	if (step == 0) {
		timer += TimeManager::deltaTime;
		if (timer >= 1.0f) {
			timer = 0;
			step++;
		}
	}
	if (step == 1) {
		timer += TimeManager::deltaTime;

		rankTextA.mTransform.position = { 640, 240, 0 };

		float t = min(1, timer / 1.0f);
		float fx = 1 - powf(1 - t, 4);
		
		rankTextA.mTransform.scale = { fx, fx, 1 };
		rankTextA.mTransform.UpdateMatrix();

		if (timer >= 1.0f) {
			timer = 0;
			step++;
		}
	}
	if (step == 2) {
		timer += TimeManager::deltaTime;

		rankTextB.mTransform.position = { 640, 320, 5 };

		if (timer <= 1.25f) {
			float t = min(1, timer / 1.25f);
			float fx = 1 - powf(1 - t, 5);

			rankTextB.mTransform.scale = { 1.0f * fx, 1.0f * fx, 1 };
			rankTextB.mTransform.UpdateMatrix();
		}
		else {
			float t = min(1, (timer - 1.25f) / 1.0f);
			float fx = 1 - powf(1 - t, 4);
			float scale = 1.0f * (1 - fx) + 0.5f * fx;

			rankTextB.mTransform.scale = { scale, scale, 1 };
			rankTextB.mTransform.UpdateMatrix();
		}

		if (timer >= 3.0f) {
			timer = 0;
			step++;
		}
	}
	if (step == 3) {
		timer += TimeManager::deltaTime;

		float t = min(1, timer / 1.0f);
		float fx = 1 - powf(1 - t, 4);

		scoreTextA.mTransform.position = Vector3(640, -100, 0) * (1 - fx) + Vector3(640, 110, 0) * fx;
		scoreTextA.mTransform.UpdateMatrix();
		scoreTextB.mTransform.position = Vector3(640, -100, 0) * (1 - fx) + Vector3(640, 160, 0) * fx;
		scoreTextB.mTransform.UpdateMatrix();

		countPerfectText.mTransform.position = Vector3(640, 820, 0) * (1 - fx) + Vector3(640, 450, 0) * fx;
		countPerfectText.mTransform.UpdateMatrix();
		countHitText.mTransform.position = Vector3(640, 820, 0) * (1 - fx) + Vector3(640, 475, 0) * fx;
		countHitText.mTransform.UpdateMatrix();
		countMissText.mTransform.position = Vector3(640, 820, 0) * (1 - fx) + Vector3(640, 500, 0) * fx;
		countMissText.mTransform.UpdateMatrix();

		goRetryButton.pos = Vector2(440, 820) * (1 - fx) + Vector2(440, 620) * fx;
		goTitleButton.pos = Vector2(840, 820) * (1 - fx) + Vector2(840, 620) * fx;

		if (timer >= 1.0f) {
			timer = 0;
			step++;
		}
	}

	if (step >= 3) {
		goRetryButton.Update();
		goTitleButton.Update();

		if (goRetryButton.isClick) {
			if (!SceneManager::IsSceneChanging()) {
				SceneManager::Change<GameScene, SimpleSceneTransition>();
			}
		}
		if (goTitleButton.isClick) {
			if (!SceneManager::IsSceneChanging()) {
				SceneManager::Change<TitleScene, SimpleSceneTransition>();
			}
		}
	}

	
}

void ResultScene::Draw()
{
	SimpleDrawer::DrawString(30, 30, 0, "Result", { 1, 0.5f, 0.5f, 1 }, "", 32, { 0.0f, 0.5f });

	if (step >= 1) {
		rankTextA.TransferBuffer();
		rankTextA.Draw();
	}
	if (step >= 2) {
		rankTextB.TransferBuffer();
		rankTextB.Draw();
	}
	if (step >= 3) {
		scoreTextA.TransferBuffer();
		scoreTextB.TransferBuffer();
		countPerfectText.TransferBuffer();
		countHitText.TransferBuffer();
		countMissText.TransferBuffer();
		scoreTextA.Draw();
		scoreTextB.Draw();
		countPerfectText.Draw();
		countHitText.Draw();
		countMissText.Draw();

		goRetryButton.Draw();
		SimpleDrawer::DrawString(goRetryButton.pos.x, goRetryButton.pos.y, 5, "リトライ", {0, 0, 0, 1}, "", 32, {0.5f, 0.5f});
		goTitleButton.Draw();
		SimpleDrawer::DrawString(goTitleButton.pos.x, goTitleButton.pos.y, 5, "終了", {0, 0, 0, 1}, "", 32, {0.5f, 0.5f});
	}
}

void ResultScene::Button::Update()
{
	Vector2 mouse = RInput::GetMousePos();

	Vector2 min = { pos.x - size.x, pos.y - size.y };
	Vector2 max = { pos.x + size.x, pos.y + size.y };

	if (min.x <= mouse.x && mouse.x <= max.x
		&& min.y <= mouse.y && mouse.y <= max.y) {
		isHover = true;
		if (RInput::GetMouseClick(0)) {
			isClick = true;
		}
	}
	else {
		isHover = false;
		isClick = false;
	}
}

void ResultScene::Button::Draw()
{
	SimpleDrawer::DrawBox(
		static_cast<int32_t>(pos.x - size.x),
		static_cast<int32_t>(pos.y - size.y),
		static_cast<int32_t>(pos.x + size.x),
		static_cast<int32_t>(pos.y + size.y),
		0, color, true);
	if (isHover) {
		SimpleDrawer::DrawBox(
			static_cast<int32_t>(pos.x - size.x),
			static_cast<int32_t>(pos.y - size.y),
			static_cast<int32_t>(pos.x + size.x),
			static_cast<int32_t>(pos.y + size.y),
			0, {0, 0, 0, 0.2f}, true);
	}
	if (isClick) {
		SimpleDrawer::DrawBox(
			static_cast<int32_t>(pos.x - size.x),
			static_cast<int32_t>(pos.y - size.y),
			static_cast<int32_t>(pos.x + size.x),
			static_cast<int32_t>(pos.y + size.y),
			0, { 0, 0, 0, 0.5f }, true);
	}
}
