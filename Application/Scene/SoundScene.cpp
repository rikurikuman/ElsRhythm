#include "SoundScene.h"
#include "RImGui.h"
#include "RInput.h"
#include <Quaternion.h>
#include <RAudio.h>
#include <complex>
#include <SimpleDrawer.h>

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
	AudioData* data = RAudio::GetData("TestScale");
	WaveAudio* audio = static_cast<WaveAudio*>(data);

	float height = 300;
	int32_t sampleRange = 22050;
	int32_t sampleStep = 50;
	int32_t nowSample = static_cast<int32_t>(RAudio::GetCurrentPosition("TestScale") * audio->wfex.nSamplesPerSec);

	float x = FLT_MAX;
	float y1 = FLT_MAX;
	float y2 = FLT_MAX;
	for (int i = 0; i < sampleRange; i += sampleStep) {
		int32_t sample = nowSample - sampleRange / 2 + i;
		int32_t block = audio->wfex.nBlockAlign * sample;
		BYTE* nowByte = nullptr;

		float freq1 = 0;
		float freq2 = 0;

		if (block >= 0 && block < audio->buffer.size()) {
			nowByte = &audio->buffer[block];

			if (audio->wfex.wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
				float* freq = reinterpret_cast<float*>(nowByte);
				freq1 = *freq;
				freq++;
				freq2 = *freq;
			}
		}

		float areaSize = height / 2;

		float area1 = 200;
		float area2 = 200 + (areaSize + 50);

		float pPos = static_cast<float>(RWindow::GetWidth() / static_cast<float>(sampleRange) * i);
		float wPos1 = area1 + areaSize * freq1;
		float wPos2 = area2 + areaSize * freq2;

		if (x == FLT_MAX) {
			SimpleDrawer::DrawLine(
				-1,
				static_cast<int32_t>(area1),
				static_cast<int32_t>(pPos),
				static_cast<int32_t>(wPos1), 100, 0xff0000);
			SimpleDrawer::DrawLine(
				-1,
				static_cast<int32_t>(area2),
				static_cast<int32_t>(pPos),
				static_cast<int32_t>(wPos2), 100, 0xff0000);
		}
		else {
			SimpleDrawer::DrawLine(
				static_cast<int32_t>(x),
				static_cast<int32_t>(y1),
				static_cast<int32_t>(pPos),
				static_cast<int32_t>(wPos1), 100, 0xff0000);
			SimpleDrawer::DrawLine(
				static_cast<int32_t>(x),
				static_cast<int32_t>(y2),
				static_cast<int32_t>(pPos),
				static_cast<int32_t>(wPos2), 100, 0xff0000);
		}
		x = pPos;
		y1 = wPos1;
		y2 = wPos2;
	}

	SimpleDrawer::DrawLine(640, 0, 640, 720, 100, 0xffffff);

	/*int32_t nowSample = static_cast<int32_t>(time * audio->wfex.nSamplesPerSec);
	int32_t nowBlock = audio->wfex.nBlockAlign * nowSample;
	BYTE* nowByte = nullptr;

	int32_t freqD1 = 0;
	int32_t freqD2 = 0;
	float freqF1 = 0;
	float freqF2 = 0;

	if (nowBlock >= 0 && nowBlock < audio->buffer.size()) {
		nowByte = &audio->buffer[nowBlock];

		if (audio->wfex.wFormatTag == WAVE_FORMAT_PCM) {
			int32_t* freq = reinterpret_cast<int32_t*>(nowByte);
			freqD1 = *freq;
			freq++;
			freqD2 = *freq;
		}
		else if (audio->wfex.wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
			float* freq = reinterpret_cast<float*>(nowByte);
			freqF1 = *freq;
			freq++;
			freqF2 = *freq;
		}
	}*/

	//テストGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 300 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Sound Control", NULL, window_flags);

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

		ImGui::InputFloat("Time", &time);
		ImGui::Text(Util::StringFormat("%d", nowSample).c_str());
		//ImGui::Text(Util::StringFormat("%d", nowBlock).c_str());
		//ImGui::Text(Util::StringFormat("%p", nowByte).c_str());
		//ImGui::Text(Util::StringFormat("%d", freqD1).c_str());
		//ImGui::Text(Util::StringFormat("%d", freqD2).c_str());
		//ImGui::Text(Util::StringFormat("%f", freqF1).c_str());
		//ImGui::Text(Util::StringFormat("%f", freqF2).c_str());
		ImGui::Spacing();
		ImGui::Text(Util::StringFormat("cbSize:%d", audio->wfex.cbSize).c_str());
		ImGui::Text(Util::StringFormat("nSamplesPerSec:%d", audio->wfex.nSamplesPerSec).c_str());
		ImGui::Text(Util::StringFormat("wBitsPerSample:%d", audio->wfex.wBitsPerSample).c_str());
		ImGui::Text(Util::StringFormat("nChannels:%d", audio->wfex.nChannels).c_str());
		ImGui::Text(Util::StringFormat("nBlockAlign:%d", audio->wfex.nBlockAlign).c_str());

		ImGui::End();
	}

	
}

void SoundScene::Draw()
{
}
