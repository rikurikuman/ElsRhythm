#pragma once
#include "Camera.h"
#include "ViewProjection.h"
#include "Vector2.h"
#include "RWindow.h"

class DebugCamera : public Camera
{
public:
	bool freeFlag = false;
	float moveSpeed = 0.2f;
	float sensitivity = 10.0f;
	Vector2 angle = { 90, 0 };

	DebugCamera(Vector3 pos, float moveSpeed = 0.2f, float sensitivity = 10.0f) : moveSpeed(moveSpeed), sensitivity(sensitivity) {
		viewProjection.eye = pos;
		viewProjection.aspect = (float)RWindow::GetWidth() / RWindow::GetHeight();
	}

	void Update() override;
};

