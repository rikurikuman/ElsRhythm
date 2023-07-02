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
	ViewProjection mViewProjection;
	RConstBuffer<ViewProjectionBuffer> mBuff;

	Camera() {
		mViewProjection.mAspect = (float)RWindow::GetWidth() / RWindow::GetHeight();
		mViewProjection.UpdateMatrix();
		mViewProjection.Transfer(mBuff.mConstMap);
	}
	virtual ~Camera() {}

	virtual void Update() {}

	static Camera* sNowCamera;
};

