#pragma once
class IBackGround
{
public:
	virtual ~IBackGround() {}

	virtual void Update() {}
	virtual void Draw() {}
};

