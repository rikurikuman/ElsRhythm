#pragma once
#include <memory>
#include "ViewProjection.h"
#include "RWindow.h"
#include "RConstBuffer.h"

class Camera
{
protected:
	//ViewProjection viewProjection;

public:
	ViewProjection viewProjection;
	RConstBuffer<ViewProjectionBuffer> buff;

	Camera() {
		viewProjection.aspect = (float)RWindow::GetWidth() / RWindow::GetHeight();
		viewProjection.UpdateMatrix();
		viewProjection.Transfer(buff.constMap);
	}
	virtual ~Camera() {}

	virtual void Update() {}

	static Camera* nowCamera;
};

