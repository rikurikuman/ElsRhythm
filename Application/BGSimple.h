#pragma once
#include "IBackGround.h"
#include "Sprite.h"
#include <cstdint>

class BGSimple : public IBackGround
{
public:
	BGSimple();

	void Update() override;
	void Draw() override;

private:
	Sprite backGround;

	float shotTimer = 0;
	int32_t shotCount = 0;
};

