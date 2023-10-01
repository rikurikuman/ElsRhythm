#include "DebugGUI.h"
#include "RImGui.h"
#include <SRBuffer.h>
#include <Util.h>
#include <SceneManager.h>
#include <TitleScene.h>
#include <GameScene.h>
#include <ResultScene.h>
#include <MainTestScene.h>
#include <ControllerScene.h>
#include <SoundScene.h>
#include <SimpleSceneTransition.h>
#include <Renderer.h>

void DebugGUI::Show()
{
	ImGui::SetNextWindowSize({ 400, 380 }, ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	ImGui::Begin("Debug", NULL, window_flags);

	ImGui::Text("SceneManager");
	static int32_t sceneNum = 0;
	const char* scenes[] = { "Title", "RhythmGameTest", "ResultTest", "MainTest", "ControllerTest", "SoundTest" };
	ImGui::Combo("##SceneNumCombo", &sceneNum, scenes, IM_ARRAYSIZE(scenes));
	ImGui::SameLine();
	if (ImGui::Button("Go!!!")) {
		if (!SceneManager::IsSceneChanging()) {
			switch (sceneNum) {
			case 0:
				SceneManager::Change<TitleScene, SimpleSceneTransition>();
				break;
			case 1:
				GameScene::sChartName = "test.kasu";
				SceneManager::Change<GameScene, SimpleSceneTransition>();
				break;
			case 2:
				SceneManager::Change<ResultScene, SimpleSceneTransition>();
				break;
			case 3:
				SceneManager::Change<MainTestScene, SimpleSceneTransition>();
				break;
			case 4:
				SceneManager::Change<ControllerScene, SimpleSceneTransition>();
				break;
			case 5:
				SceneManager::Change<SoundScene, SimpleSceneTransition>();
				break;
			}
		}
	}

	ImGui::NewLine();
	ImGui::Separator();

	if (ImGui::BeginTabBar("DebugTabBar")) {
		if (ImGui::BeginTabItem("Info")) {
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Spacing();
			ImGui::Text("MousePos:(%.1f,%.1f)", ImGui::GetMousePos().x, ImGui::GetMousePos().y);
			ImGui::Spacing();

			ImGui::NewLine();
			ImGui::Spacing();
			ImGui::Text("DebugCamera Control");
			ImGui::Text("* Valid only in available scenes.");
			ImGui::Text("Left Control Key : Enable / Disable");
			ImGui::Text("WASD Key : Move");
			ImGui::Text("Space Key : Up");
			ImGui::Text("LShift Key : Down");
			ImGui::Text("Mouse : Direction");

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Buffer")) {
			UINT64 buffUsingSize = SRBufferAllocator::GetUsingBufferSize();
			UINT64 buffTotalSize = SRBufferAllocator::GetBufferSize();
			std::string buffUsingSizeStr;
			std::string buffTotalSizeStr;
			if (buffUsingSize < 1024 * 1024 - 1) {
				buffUsingSizeStr = Util::StringFormat("%.2fKB", buffUsingSize / 1024.0f);
			}
			else {
				buffUsingSizeStr = Util::StringFormat("%.2fMB", buffUsingSize / 1024.0f / 1024.0f);
			}
			if (buffTotalSize < 1024 * 1024 - 1) {
				buffTotalSizeStr = Util::StringFormat("%.2fKB", buffTotalSize / 1024.0f);
			}
			else {
				buffTotalSizeStr = Util::StringFormat("%.2fMB", buffTotalSize / 1024.0f / 1024.0f);
			}
			ImGui::Text("SRBuffer");
			ImGui::Text(("Using : " + buffUsingSizeStr + " / " + buffTotalSizeStr + "(%.2lf%%)").c_str(), static_cast<double>(buffUsingSize) / buffTotalSize * 100);
			ImGui::Text("(Raw : %lld / %lld)", buffUsingSize, buffTotalSize);
			ImGui::ProgressBar(static_cast<float>(buffUsingSize) / buffTotalSize);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Renderer")) {
			// Left
			std::vector<std::string> stages = Renderer::GetRenderStageIDs();
			static int32_t selected = 0;
			{
				ImVec2 parentSize = ImGui::GetWindowSize();
				ImGui::BeginChild("left pane", ImVec2(parentSize.x / 3.0f, 0), true, ImGuiWindowFlags_AlwaysAutoResize);
				for (int32_t i = 0; i < stages.size(); i++)
				{
					IRenderStage* stage = Renderer::GetRenderStage(stages[i]);
					if (ImGui::Selectable(Util::StringFormat("##%s", stage->GetTypeIndentifier().c_str()).c_str(), selected == i)) {
						selected = i;
					}
					ImGui::SameLine();
					if (stage->mFlagEnabled) {
						ImGui::Text((stage->GetTypeIndentifier() + Util::StringFormat(" (%d)", stage->mOrders.size())).c_str());
					}
					else {
						ImGui::TextDisabled((stage->GetTypeIndentifier() + Util::StringFormat(" (%d)", stage->mOrders.size())).c_str());
					}
				}
				ImGui::EndChild();
			}
			ImGui::SameLine();

			// Right
			{
				IRenderStage* stage = Renderer::GetRenderStage(stages[selected]);
				ImGui::BeginGroup();
				ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
				ImGui::Text(stage->GetTypeIndentifier().c_str());
				ImGui::Separator();
				ImGui::Checkbox("Enable", &stage->mFlagEnabled);
				ImGui::Text(Util::StringFormat("OrderCount:%d", stage->mOrders.size()).c_str());
				ImGui::EndChild();
				ImGui::EndGroup();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	
	ImGui::End();
}
