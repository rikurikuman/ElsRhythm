#pragma once
#include <memory>
#include <future>
#include <vector>
#include "IScene.h"
#include "ISceneTransition.h"
#include "Sprite.h"

class SceneManager
{
public:
	static SceneManager* GetInstance();
	static IScene* GetNowScene();

	static bool IsSceneChanging();
	static bool CheckGameEndFlag();

	static void Finalize();
	static void Update();
	static void Draw();

private:
	struct SceneChange {
		std::shared_ptr<std::future<std::shared_ptr<IScene>>> future = nullptr;
		std::shared_ptr<std::future<bool>> future2 = nullptr;
		std::shared_ptr<ISceneTransition> transition = nullptr;
		std::shared_ptr<IScene> scene = nullptr;
		int increment = 0;
	};

	std::mutex mutex;
	std::shared_ptr<IScene> nowScene = nullptr;
	std::shared_ptr<IScene> changeScene = nullptr;
	std::vector<SceneChange> runningSceneChange;
	bool gameEnd = false;

	float loadingTimer = 0;
	float loadingAngle = 0;
	float loadingAlpha = 0;
	Sprite loadingMark;

	SceneManager();
	~SceneManager() = default;
	SceneManager(const SceneManager& a) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

public:
	template<class SceneClassName, class SceneTransitionClassName = ISceneTransition>
	static void Change() {
		SceneManager* instance = GetInstance();

		SceneChange sc{};
		sc.future = std::make_shared<std::future<std::shared_ptr<IScene>>>(std::async(std::launch::async, [] {
			std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::mutex);
			SceneManager* instance = GetInstance();
			std::shared_ptr<IScene> newScene = std::make_shared<SceneClassName>();
			return newScene;
		}));

		sc.transition = std::make_shared<SceneTransitionClassName>();
		instance->runningSceneChange.push_back(sc);
	}

	template<typename SceneClassName>
	static void Set() {
		SceneManager* instance = GetInstance();
		instance->runningSceneChange.clear();
		std::unique_ptr<IScene> newScene = std::make_unique<SceneClassName>();
		instance->nowScene = std::move(newScene);
		instance->nowScene->Init();
	}
};