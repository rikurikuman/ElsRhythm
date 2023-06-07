#pragma once
class IScene
{
public:
	virtual ~IScene() {}

	virtual void Init() {}
	virtual void Update() {}
	virtual void Draw() {}
	virtual void Finalize() {}
};

