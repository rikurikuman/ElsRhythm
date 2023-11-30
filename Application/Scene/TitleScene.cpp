#include "TitleScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>
#include <SceneManager.h>
#include <SelectScene.h>
#include <SimpleSceneTransition.h>
#include <ParticleObject.h>

TitleScene::TitleScene()
{
	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	bgHexa1.SetTexture(TextureManager::Load("./Resources/ptHexagon.png", "BackHexa"));
	bgHexa2.SetTexture(TextureManager::Load("./Resources/ptHexagon.png", "BackHexa"));
	bgHexa1.mTransform.position = { 265, 530, 0 };
	bgHexa2.mTransform.position = { 1035, 200, 0 };
	bgHexa1.mTransform.scale = { 1.5f, 1.5f, 1.0f };
	bgHexa2.mTransform.scale = { 2.0f, 2.0f, 1.0f };
	bgHexa1.mMaterial.mColor = { 0, 0.6f, 0.6f, 1 };
	bgHexa2.mMaterial.mColor = { 0, 0.6f, 0.6f, 1 };
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

	if (GetActiveWindow() == RWindow::GetWindowHandle()
		&& !ImGui::GetIO().WantCaptureMouse
		&& RInput::GetMouseClickDown(0)
		&& !SceneManager::IsSceneChanging()) {
		SceneManager::Change<SelectScene, SimpleSceneTransition>();
	}
}

void TitleScene::Draw()
{
	SimpleDrawer::DrawString(RWindow::GetWidth() / 2.0f, 100, 5, "リズムマウサー", {1, 1, 1, 1}, "やさしさゴシックボールドV2", 128, {0.5f, 0.5f});
	SimpleDrawer::DrawString(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 10, "画面をクリックでスタート！", {1, 1, 1, 1}, "やさしさゴシックボールドV2", 32, {0.5f, 0.5f});

	bgHexa1.mTransform.rotation.z += Util::AngleToRadian(90) * TimeManager::deltaTime;
	bgHexa1.mTransform.UpdateMatrix();
	bgHexa1.TransferBuffer();
	bgHexa1.Draw();
	bgHexa2.mTransform.rotation.z -= Util::AngleToRadian(120) * TimeManager::deltaTime;
	bgHexa2.mTransform.UpdateMatrix();
	bgHexa2.TransferBuffer();
	bgHexa2.Draw();
}