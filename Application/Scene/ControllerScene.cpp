#include "ControllerScene.h"
#include "RImGui.h"
#include "RInput.h"
#include <Quaternion.h>

ControllerScene::ControllerScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome", "Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();

	model = ModelObj(Model::Load("./Resources/Model/VicViper", "VicViper.obj", "Vic", true));

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void ControllerScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void ControllerScene::Update()
{
	//テストGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 150 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Model Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			model.mTransform.position = { 0, 0, 0 };
			model.mTransform.rotation = { 0, 0, 0 };
		}

		ImGui::End();
	}

	model.mTransform.rotation += { 0, Util::AngleToRadian(5) * RInput::GetLStick(true, true).x, 0 };

	Vector3 vec = { 0, 0, 1 };
	vec *= Matrix4::RotationZXY(model.mTransform.rotation.x, model.mTransform.rotation.y, model.mTransform.rotation.z);
	vec.Normalize();
	model.mTransform.position += vec * 3 * RInput::GetLStick(true, true).y;
	model.mTransform.UpdateMatrix();

	Vector3 targetPos = model.mTransform.position + vec * 5.0f;

	Vector3 cameraVec = -vec;
	cameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(vec), Util::AngleToRadian(20));
	cameraVec *= 20.0f;

	Vector3 diffPos = (model.mTransform.position + cameraVec) - camera.mViewProjection.mEye;
	Vector3 diffTarget = targetPos - camera.mViewProjection.mTarget;

	camera.mViewProjection.mEye += diffPos / 5;
	camera.mViewProjection.mTarget += diffTarget / 5;
	camera.mViewProjection.UpdateMatrix();

	light.Update();

	skydome.TransferBuffer(camera.mViewProjection);
	model.TransferBuffer(camera.mViewProjection);
}

void ControllerScene::Draw()
{
	skydome.Draw();
	model.Draw();
}
