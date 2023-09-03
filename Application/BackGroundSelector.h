#pragma once
#include <string>
#include <memory>
#include "IBackGround.h"

class BackGroundSelector
{
public:
	static void Select(std::string name);

	static void Update();
	static void Draw();

private:
	std::unique_ptr<IBackGround> bg;
	static BackGroundSelector* GetInstance();
};

