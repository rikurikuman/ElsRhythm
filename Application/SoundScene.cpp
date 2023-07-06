#include "SoundScene.h"
#include "RImGui.h"
#include "RInput.h"
#include <Quaternion.h>
#include <RAudio.h>

SoundScene::SoundScene()
{
	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void SoundScene::Init()
{
	RAudio::Load("Resources/Test.wav", "TestScale");
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void SoundScene::Update()
{
	//ƒeƒXƒgGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 300 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Model Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			loop = false;
			playRange = { 0, 0 };
			loopRange = { 0, 0 };
		}

		ImGui::Separator();

		ImGui::Checkbox("Loop", &loop);
		ImGui::InputFloat("PlayStart", &playRange.x);
		ImGui::InputFloat("PlayEnd", &playRange.y);
		ImGui::InputFloat("LoopStart", &loopRange.x);
		ImGui::InputFloat("LoopEnd", &loopRange.y);
		ImGui::Text(Util::StringFormat("IsPlaying:%d", RAudio::IsPlaying("TestScale")).c_str());
		ImGui::Text(Util::StringFormat("Current:%f", RAudio::GetCurrentPosition("TestScale")).c_str());

		if (ImGui::Button("Play")) {
			RAudio::SetPlayRange("TestScale", playRange.x, playRange.y);
			RAudio::SetLoopRange("TestScale", loopRange.x, loopRange.y);
			RAudio::Play("TestScale", 1, 1, loop);
		}
		if (ImGui::Button("Stop")) {
			RAudio::Stop("TestScale");
		}

		ImGui::End();
	}
}

void SoundScene::Draw()
{
}
