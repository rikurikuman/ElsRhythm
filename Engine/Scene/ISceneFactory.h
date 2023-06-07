#pragma once
#include <memory>
#include <string>
#include "IScene.h"

class ISceneFactory
{
public:
	virtual ~ISceneFactory() = default;

	virtual std::shared_ptr<IScene> CreateScene(const std::string& sceneName) {
		return nullptr;
	}
};

