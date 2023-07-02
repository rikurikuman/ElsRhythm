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
	return GetInstance()->mNowScene.get();
}

bool SceneManager::IsSceneChanging() {
	return !GetInstance()->mRunningSceneChanges.empty();
}

bool SceneManager::CheckGameEndFlag()
{
	return GetInstance()->mFlagGameEnd;
}

void SceneManager::Finalize()
{
	GetInstance()->mFlagGameEnd = true;
	GetInstance()->mRunningSceneChanges.clear();
}

SceneManager::SceneManager() {
	mLoadingMark = Sprite(TextureManager::Load("Resources/loadingMark.png", "LoadingMark"));
}

void SceneManager::Update() {
	SceneManager* instance = GetInstance();

	if (!instance->mRunningSceneChanges.empty()) {
		SceneChange& sc = *instance->mRunningSceneChanges.begin();

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
				std::lock_guard<std::recursive_mutex> lockSR(SRBufferAllocator::sMutex);
				SceneManager* instance = GetInstance();
				std::lock_guard<std::mutex> lock(instance->mMutex);
				std::shared_ptr<IScene> scene = std::move(sc.scene);
				std::swap(instance->mChangeScene, scene);
				return true;
			}));
			sc.increment++;
		}

		if (sc.increment == 2
			&& sc.transition->IsClosed()
			&& sc.future2->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			std::lock_guard<std::mutex> lock(instance->mMutex);
			auto trash = std::make_shared<IScene>();
			instance->mNowScene->Finalize();
			std::swap(instance->mNowScene, instance->mChangeScene);
			std::swap(instance->mChangeScene, trash);
			instance->mNowScene->Init();
			instance->mLoadingTimer = 0;
			sc.transition->Open();
			sc.increment++;
		}

		if (sc.increment == 3
			&& sc.transition->IsOpened()) {
			instance->mRunningSceneChanges.erase(instance->mRunningSceneChanges.begin());
		}

		//ローディングマーク
		if (sc.increment == 1 && sc.transition->IsClosed()) {
			instance->mLoadingTimer += TimeManager::deltaTime;
			if (instance->mLoadingTimer >= 0.5f) {
				float t = min(1, instance->mLoadingTimer - 0.5f / 0.5f);
				float fx = 1 - powf(1 - t, 5);

				instance->mLoadingAngle += Util::AngleToRadian(5);
				instance->mLoadingMark.mTransform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->mLoadingMark.mTransform.rotation = { 0, 0, instance->mLoadingAngle };
				instance->mLoadingMark.mTransform.UpdateMatrix();
				instance->mLoadingMark.mMaterial.mColor.a = 1.0f * fx;
				instance->mLoadingAlpha = 1.0f * fx;
			}
			else {
				instance->mLoadingAngle = 0;
				instance->mLoadingMark.mTransform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->mLoadingMark.mTransform.rotation = { 0, 0, 0 };
				instance->mLoadingMark.mTransform.UpdateMatrix();
				instance->mLoadingMark.mMaterial.mColor.a = 0;
			}
		}
		if (sc.increment == 3) {
			if (instance->mLoadingAngle != 0) {
				instance->mLoadingTimer += TimeManager::deltaTime;
				float t = min(1, instance->mLoadingTimer / 0.5f);
				float fx = 1 - powf(1 - t, 5);

				instance->mLoadingAngle += Util::AngleToRadian(5);
				instance->mLoadingMark.mTransform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->mLoadingMark.mTransform.rotation = { 0, 0, instance->mLoadingAngle };
				instance->mLoadingMark.mTransform.UpdateMatrix();
				instance->mLoadingMark.mMaterial.mColor.a = instance->mLoadingAlpha * (1 - fx);
			}
		}
	}
	else {
		instance->mLoadingTimer = 0;
		instance->mLoadingAngle = 0;
		instance->mLoadingAlpha = 0;
	}

	if (instance->mNowScene != nullptr) {
		instance->mNowScene->Update();
	}
}

void SceneManager::Draw() {
	SceneManager* instance = GetInstance();
	if (instance->mNowScene != nullptr) {
		instance->mNowScene->Draw();
	}
	if (!instance->mRunningSceneChanges.empty()) {
		SceneChange& sc = *instance->mRunningSceneChanges.begin();
		sc.transition->Draw();

		if (instance->mLoadingAlpha != 0 && ((sc.increment == 1 && sc.transition->IsClosed()) || sc.increment == 2 || sc.increment == 3)) {
			instance->mLoadingMark.TransferBuffer();
			instance->mLoadingMark.Draw();
		}
	}
}