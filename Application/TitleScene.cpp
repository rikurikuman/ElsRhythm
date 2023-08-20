#include "TitleScene.h"
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

TitleScene::TitleScene()
{
	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	goEasyButton.pos = Vector2(RWindow::GetWidth() / 2.0f - 200, RWindow::GetHeight() / 2.0f);
	goEasyButton.size = Vector2(150, 100);
	goEasyButton.color = { 0, 1, 0, 1 };

	goHardButton.pos = Vector2(RWindow::GetWidth() / 2.0f + 200, RWindow::GetHeight() / 2.0f);
	goHardButton.size = Vector2(150, 100);
	goHardButton.color = { 1, 0, 0, 1 };
}

void TitleScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
	ParticleObject::Clear();
}

void TitleScene::Update()
{
	light.Update();
	camera.Update();

	goEasyButton.Update();
	goHardButton.Update();

	if (goEasyButton.isClick) {
		if (!SceneManager::IsSceneChanging()) {
			GameScene::sChartName = "aleph-0.kasu";
			SceneManager::Change<GameScene, SimpleSceneTransition>();
		}
	}
	if (goHardButton.isClick) {
		if (!SceneManager::IsSceneChanging()) {
			GameScene::sChartName = "aleph-0_.kasu";
			SceneManager::Change<GameScene, SimpleSceneTransition>();
		}
	}
}

void TitleScene::Draw()
{
	SimpleDrawer::DrawString(RWindow::GetWidth() / 2.0f, 100, 0, "リズムPCマスター", { 1, 1, 1, 1 }, "", 64, {0.5f, 0.5f});
	goEasyButton.Draw();
	SimpleDrawer::DrawString(goEasyButton.pos.x, goEasyButton.pos.y, 5, "やさしめモード", { 0, 0, 0, 1 }, "", 32, { 0.5f, 0.5f });
	goHardButton.Draw();
	SimpleDrawer::DrawString(goHardButton.pos.x, goHardButton.pos.y, 5, "つよすぎモード", { 0, 0, 0, 1 }, "", 32, { 0.5f, 0.5f });
}

void TitleScene::Button::Update()
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

void TitleScene::Button::Draw()
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
