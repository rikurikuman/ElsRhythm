#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>

MainTestScene::MainTestScene()
{
	sphere = ModelObj(Model::LoadWithAIL("./Resources/Model/", "Sphere.obj", "Sphere"));

	sphere.transform.position = { 0, 0, 0 };
	sphere.transform.UpdateMatrix();

	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere2", true));

	sphere2.transform.position = { 0.5f, 0, -1 };
	sphere2.transform.UpdateMatrix();

	sphere2.tuneMaterial.color.a = 0.6f;

	sprite.SetTexture("");
	sprite.SetAnchor({ 0, 0 });
	sprite.transform.position = { 100, 100, 0 };
	sprite.transform.UpdateMatrix();
	sprite.material.color = { 1, 1, 1, 1 };

	sprite2.SetTexture("");
	sprite2.SetAnchor({ 0, 0 });
	sprite2.transform.position = { 120, 120, 0 };
	sprite2.transform.UpdateMatrix();
	sprite2.material.color = { 1, 0, 0, 0.5f };

	camera.viewProjection.eye = { 0, 0, -10 };
	camera.viewProjection.target = { 0, 0, 0 };
	camera.viewProjection.UpdateMatrix();
}

void MainTestScene::Init()
{
	Camera::nowCamera = &camera;
	LightGroup::nowLight = &light;
	PostEffect::InitPipeline();
	hoge.SetTexture("RenderTargetTex_hoge");
}

void MainTestScene::Update()
{
	light.Update();
	camera.Update();

	timer += TimeManager::deltaTime;
	if (timer >= 0.01f) {
		timer = 0;
		for (int i = 0; i < 10; i++) {
			TestObj obj{};
			obj.obj = ModelObj("Sphere");
			obj.speed = Vector3(Util::GetRand(-1.0f, 1.0f), Util::GetRand(-1.0f, 1.0f), Util::GetRand(-1.0f, 1.0f));
			obj.timer = 0;
			testObjList.push_back(obj);
		}
	}

	for (auto itr = testObjList.begin(); itr != testObjList.end();) {
		TestObj& obj = *itr;
		obj.timer += TimeManager::deltaTime;
		if (obj.timer >= 5) {
			itr = testObjList.erase(itr);
			continue;
		}
		obj.obj.transform.position += obj.speed;
		obj.obj.transform.UpdateMatrix();
		obj.obj.TransferBuffer(camera.viewProjection);
		itr++;
	}

	sphere.TransferBuffer(camera.viewProjection);
	sphere2.TransferBuffer(camera.viewProjection);
	sprite.TransferBuffer();
	sprite2.TransferBuffer();
	hoge.TransferBuffer();
}

void MainTestScene::Draw()
{
	sphere.Draw();
	sphere2.Draw();

	sprite.Draw();
	sprite2.Draw();

	for (auto itr = testObjList.begin(); itr != testObjList.end(); itr++) {
		TestObj& obj = *itr;
		obj.obj.Draw();
	}

	SimpleDrawer::DrawString(0, 0, 0, Util::StringFormat("Count:%d", testObjList.size()), { 1, 1, 1, 1 }, "", 20);

	for (int i = 0; i < 10; i++) {
		//SimpleDrawer::DrawCircle(Util::GetRand(0, RWindow::GetWidth()), Util::GetRand(0, RWindow::GetHeight()), 5, Color(1, 0, 1, 1));
	}
}