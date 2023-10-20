#pragma once
#include <string>
#include <memory>
#include "IBackGround.h"

class BackGroundSelector
{
public:
	void Select(std::string name);

	void Update();
	void Draw();

private:
	std::unique_ptr<IBackGround> bg;
};

