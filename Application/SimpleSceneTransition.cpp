#include "SimpleSceneTransition.h"
#include "RWindow.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"

SimpleSceneTransition::SimpleSceneTransition() {
	TextureManager::Create({ 1, 1, 1, 1 }, 100, 100, "Shutter");
	shutter = Sprite("Shutter", {0.5f, 0.5f});
}

void SimpleSceneTransition::Update()
{
	if (closeFlag) {
		if (shutterScale >= 1) {
			inProgress = false;
			shutterScale = 1;
			shutterAlpha = 1;
		}
		else {
			inProgress = true;
			timer += TimeManager::deltaTime;
			float t = min(1.0f, timer / 0.8f);

			shutterScale = (1.0f - powf(1 - t, 5));
			shutterAlpha = 1;
		}
	}
	else {
		if (shutterAlpha <= 0) {
			inProgress = false;
			shutterAlpha = 0;
		}
		else {
			inProgress = true;
			timer += TimeManager::deltaTime;
			float t = min(1.0f, timer / 0.8f);

			shutterAlpha = 1 - (1.0f - powf(1 - t, 5));
		}
	}

	float scale = 15.0f * shutterScale;
	shutter.transform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 1145141919.0f };
	shutter.transform.rotation = { 0, 0, Util::AngleToRadian(180 * shutterScale)};
	shutter.transform.scale = { scale, scale, 1 };
	shutter.material.color = { 0, 0.5f, 0.7f, shutterAlpha };
	shutter.transform.UpdateMatrix();
	shutter.TransferBuffer();
}

void SimpleSceneTransition::Draw()
{
	if (shutterScale != 0) {
		shutter.Draw();
		//SimpleDrawer::DrawBox(0, 0, (float)RWindow::GetWidth(), (float)RWindow::GetHeight() * shutterPos, Color(0, 0.5f, 0.5f, 1), true);
	}
}

void SimpleSceneTransition::Open()
{
	closeFlag = false;
	timer = 0;
}

bool SimpleSceneTransition::IsOpened()
{
	return !closeFlag && !inProgress && timer != 0;
}

void SimpleSceneTransition::Close()
{
	closeFlag = true;
	timer = 0;
}

bool SimpleSceneTransition::IsClosed()
{
	return closeFlag && !inProgress && timer != 0;
}

bool SimpleSceneTransition::InProgress()
{
	return inProgress;
}