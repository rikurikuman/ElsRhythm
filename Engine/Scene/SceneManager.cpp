#include "SceneManager.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include "Util.h"
#include "RWindow.h"
#include "TimeManager.h"

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

IScene* SceneManager::GetNowScene() {
	return GetInstance()->nowScene.get();
}

bool SceneManager::IsSceneChanging() {
	return !GetInstance()->runningSceneChange.empty();
}

bool SceneManager::CheckGameEndFlag()
{
	return GetInstance()->gameEnd;
}

void SceneManager::Finalize()
{
	GetInstance()->gameEnd = true;
	GetInstance()->runningSceneChange.clear();
}

SceneManager::SceneManager() {
	loadingMark = Sprite(TextureManager::Load("Resources/loadingMark.png", "LoadingMark"));
}

void SceneManager::Update() {
	SceneManager* instance = GetInstance();

	if (!instance->runningSceneChange.empty()) {
		SceneChange& sc = *instance->runningSceneChange.begin();

		if (sc.increment == 0) {
			sc.transition->Close();
			sc.increment++;
		}
		sc.transition->Update();

		if (sc.increment == 1
				&& sc.transition->IsClosed()
				&& sc.future->valid()
				&& sc.future->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			sc.scene = sc.future->get();
			sc.future2 = std::make_shared<std::future<bool>>(std::async(std::launch::async, [&sc] {
				std::lock_guard<std::recursive_mutex> lockSR(SRBufferAllocator::mutex);
				SceneManager* instance = GetInstance();
				std::lock_guard<std::mutex> lock(instance->mutex);
				std::shared_ptr<IScene> scene = std::move(sc.scene);
				std::swap(instance->changeScene, scene);
				return true;
			}));
			sc.increment++;
		}

		if (sc.increment == 2
			&& sc.transition->IsClosed()
			&& sc.future2->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			std::lock_guard<std::mutex> lock(instance->mutex);
			instance->nowScene->Finalize();
			std::swap(instance->nowScene, instance->changeScene);
			std::swap(instance->changeScene, std::make_shared<IScene>());
			instance->nowScene->Init();
			instance->loadingTimer = 0;
			sc.transition->Open();
			sc.increment++;
		}

		if (sc.increment == 3
			&& sc.transition->IsOpened()) {
			instance->runningSceneChange.erase(instance->runningSceneChange.begin());
		}

		//ローディングマーク
		if (sc.increment == 1 && sc.transition->IsClosed()) {
			instance->loadingTimer += TimeManager::deltaTime;
			if (instance->loadingTimer >= 0.5f) {
				float t = min(1, instance->loadingTimer - 0.5f / 0.5f);
				float fx = 1 - powf(1 - t, 5);

				instance->loadingAngle += Util::AngleToRadian(5);
				instance->loadingMark.transform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->loadingMark.transform.rotation = { 0, 0, instance->loadingAngle };
				instance->loadingMark.transform.UpdateMatrix();
				instance->loadingMark.material.color.a = 1.0f * fx;
				instance->loadingAlpha = 1.0f * fx;
			}
			else {
				instance->loadingAngle = 0;
				instance->loadingMark.transform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->loadingMark.transform.rotation = { 0, 0, 0 };
				instance->loadingMark.transform.UpdateMatrix();
				instance->loadingMark.material.color.a = 0;
			}
		}
		if (sc.increment == 3) {
			if (instance->loadingAngle != 0) {
				instance->loadingTimer += TimeManager::deltaTime;
				float t = min(1, instance->loadingTimer / 0.5f);
				float fx = 1 - powf(1 - t, 5);

				instance->loadingAngle += Util::AngleToRadian(5);
				instance->loadingMark.transform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->loadingMark.transform.rotation = { 0, 0, instance->loadingAngle };
				instance->loadingMark.transform.UpdateMatrix();
				instance->loadingMark.material.color.a = instance->loadingAlpha * (1 - fx);
			}
		}
	}
	else {
		instance->loadingTimer = 0;
		instance->loadingAngle = 0;
		instance->loadingAlpha = 0;
	}

	if (instance->nowScene != nullptr) {
		instance->nowScene->Update();
	}
}

void SceneManager::Draw() {
	SceneManager* instance = GetInstance();
	if (instance->nowScene != nullptr) {
		instance->nowScene->Draw();
	}
	if (!instance->runningSceneChange.empty()) {
		SceneChange& sc = *instance->runningSceneChange.begin();
		sc.transition->Draw();

		if (instance->loadingAlpha != 0 && ((sc.increment == 1 && sc.transition->IsClosed()) || sc.increment == 2 || sc.increment == 3)) {
			instance->loadingMark.TransferBuffer();
			instance->loadingMark.Draw();
		}
	}
}