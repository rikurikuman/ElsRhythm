#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>

MainTestScene::MainTestScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome", true));

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void MainTestScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void MainTestScene::Update()
{
	{
		ImGui::SetNextWindowSize({ 400, 270 }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos({ 800, 100 }, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Scene", NULL, window_flags);

		if(ImGui::Button("Reset")) {

		}

		ImGui::Separator();

		ImGui::Text("MultiRenderTest");

		ImGui::DragFloat3("ArcStart", &testObj.mStartPos.x);
		ImGui::DragFloat3("ArcGoal", &testObj.mGoalPos.x);
		
		ImGui::End();
	}


	for (int i = 0; i < 10; i++) {
		testList.emplace_back();
		testList.back().model = ModelObj(Model::Load("Resources/Model/Sphere.obj", "Sphere", true));
		testList.back().model.mTransform.scale = { 0.5f, 0.5f, 0.5f };
		testList.back().model.mTuneMaterial.mColor = Color(Util::GetRand(0.0f, 1.0f), Util::GetRand(0.0f, 1.0f), Util::GetRand(0.0f, 1.0f), 1);
		testList.back().vec = Vector3(Util::GetRand(-20.0f, 20.0f), Util::GetRand(-20.0f, 20.0f), Util::GetRand(-20.0f, 20.0f));
	}
	
	for (auto itr = testList.begin(); itr != testList.end();) {
		Test& T = *itr;
		T.timer += TimeManager::deltaTime;
		if (T.timer >= 2.0f) {
			itr = testList.erase(itr);
			continue;
		}

		T.model.mTransform.position += T.vec * TimeManager::deltaTime;
		T.model.mTransform.UpdateMatrix();
		T.model.TransferBuffer(camera.mViewProjection);
		itr++;
	}

	light.Update();
	camera.Update();

	skydome.TransferBuffer(camera.mViewProjection);
}

void MainTestScene::Draw()
{
	testObj.TransferBuffer(ViewProjection());
	testObj.Draw();
	//skydome.Draw();

	for (auto itr = testList.begin(); itr != testList.end();) {
		Test& T = *itr;
		T.model.Draw();
		itr++;
	}

	//SimpleDrawer::DrawBox(100, 100, 200, 200, 0, { 1, 1, 1, 1 }, true);
	//SimpleDrawer::DrawBox(120, 120, 220, 220, 0, { 1, 0, 0, 0.5f }, true);

	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 30, 710, 5, 0, { 1, 0, 0, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 20, 710, 5, 0, { 0, 1, 0, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 10, 710, 5, 0, { 0, 0, 1, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 60, 710, 5, 0, { 1, 0, 0, 1 }, false);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 50, 710, 5, 0, { 0, 1, 0, 1 }, false);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 40, 710, 5, 0, { 0, 0, 1, 1 }, false);
	SimpleDrawer::DrawBox(RWindow::GetWidth() - 80, 705, RWindow::GetWidth() - 70, 715, 0, { 1, 0, 0, 1 }, false, 2);
}
