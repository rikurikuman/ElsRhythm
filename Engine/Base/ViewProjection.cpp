#include "ViewProjection.h"
#include <Float4.h>
#include <RWindow.h>

void ViewProjection::UpdateMatrix()
{
	view = Matrix4::View(eye, target, up);
	if (type == Type::Perspective) {
		view = Matrix4::View(eye, target, up);
		projection = Matrix4::PerspectiveProjection(fov, aspect, nearclip, farclip);
	}
	else if (type == Type::Orthographic) {
		Vector3 _eye = eye;
		_eye.x -= orthoSize.x / 2.0f;
		_eye.y -= orthoSize.y / 2.0f;
		Vector3 _target = target;
		_target.x -= orthoSize.x / 2.0f;
		_target.y -= orthoSize.y / 2.0f;
		view = Matrix4::View(_eye, _target, up);
		projection = Matrix4::OrthoGraphicProjection(0, orthoSize.x, 0, orthoSize.y, nearclip, farclip);
	}
	matrix = view * projection;
}

void ViewProjection::Transfer(ViewProjectionBuffer* target)
{
	target->matrix = this->matrix;
	target->cameraPos = this->eye;
}

Vector2 ViewProjection::WorldToScreen(Vector3 wPos)
{
	return this->WorldToScreen(wPos,
		0, 0,
		static_cast<float>(RWindow::GetWidth()),
		static_cast<float>(RWindow::GetHeight()),
		0, 1
	);
}

Vector2 ViewProjection::WorldToScreen(Vector3 wPos, float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	Float4 screenPos = wPos;
	screenPos *= matrix;
	screenPos /= screenPos.w;
	screenPos *= Matrix4::Viewport(
		topLeftX, topLeftY,
		width, height,
		minDepth, maxDepth);
	return screenPos;
}
