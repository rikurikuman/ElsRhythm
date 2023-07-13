#include <Windows.h>
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include <string>
#include "RWindow.h"
#include "RDirectX.h"
#include "RInput.h"
#include "Texture.h"
#include "ViewProjection.h"
#include "Util.h"
#include "Model.h"
#include "DebugCamera.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "RAudio.h"
#include "SceneManager.h"
#include "SimpleSceneTransition.h"
#include "RImGui.h"
#include "MainTestScene.h"
#include "GameScene.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "Colliders.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <TestRenderStage.h>
#include <SRBuffer.h>
#include <CollidersScene.h>
#include <ControllerScene.h>
#include <SoundScene.h>

using namespace std;
using namespace DirectX;

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	//WindowsAPI
	RWindow::SetWindowName(L"RKEngine");
	RWindow::Init();

	//DirectX
	RDirectX::Init();

	//ImGui
	RImGui::Init();

	//DirectInput
	RInput::Init();

	//XAudio2
	RAudio::Init();

	TimeManager::Init();

	SRBufferAllocator::GetInstance();

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(RDirectX::GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	}
#endif

	///////////////////

	D3D12_VIEWPORT viewportS{};
	viewportS.Width = WIN_WIDTH;
	viewportS.Height = WIN_HEIGHT;
	viewportS.TopLeftX = 0;
	viewportS.TopLeftY = 0;
	viewportS.MinDepth = 0.0f;
	viewportS.MaxDepth = 1.0f;

	D3D12_RECT scissorRectS{};
	scissorRectS.left = 0;
	scissorRectS.right = scissorRectS.left + WIN_WIDTH;
	scissorRectS.top = 0;
	scissorRectS.bottom = scissorRectS.top + WIN_HEIGHT;

	//モデルデータの読み込み
	Model::Load("Resources/Model/", "Cube.obj", "Cube");

	TextureManager::Load("Resources/loadingMark.png", "LoadingMark");

	TextureManager::Load("Resources/hogetest.png", "hogetest");

	SceneManager::Set<GameScene>();

	DebugCamera camera({ 0, 0, -10 });

	TimeManager::targetFPS = 120;

	//////////////////////////////////////

	while (true) {
		RWindow::ProcessMessage();

		if (RWindow::GetInstance()->GetMessageStructure().message == WM_QUIT) {
			break;
		}

		RImGui::NewFrame();

		if (Util::instanceof<DebugCamera>(*Camera::sNowCamera)
			&& !dynamic_cast<DebugCamera*>(Camera::sNowCamera)->mFreeFlag) {
			RWindow::SetMouseLock(true);
		}
		else {
			RWindow::SetMouseLock(false);
		}

		if (RInput::GetKeyDown(DIK_F5)) {
			Util::debugBool = !Util::debugBool;
		}

		RInput::Update();
		RAudio::Update();

		SceneManager::Update();

		//以下描画処理
		RDirectX::PreDraw();
		Renderer::SetAllParamaterToAuto();

		//画面クリア～
		RDirectX::ClearBackBuffer(Color(0.0f, 0.0f, 0.0f, 1.0f));

		//描画コマンド
		SceneManager::Draw();
		SimpleDrawer::DrawAll();

		//描画の実行中に別スレッドとかでSRBufferとかの操作をされるとぶっ飛ぶので雑にロックする
		//いずれもっと良い制御にしたい
		std::unique_lock<std::recursive_mutex> bufferLockInDrawing(SRBufferAllocator::GetInstance()->sMutex);
		Renderer::Execute();

		//べんり！
		{
			ImGui::SetNextWindowSize({ 400, 380 });

			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoResize;
			ImGui::Begin("Debug", NULL, window_flags);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Spacing();
			ImGui::Text("MousePos:(%.1f,%.1f)", ImGui::GetMousePos().x, ImGui::GetMousePos().y);
			ImGui::Spacing();
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
			ImGui::NewLine();
			ImGui::Text("SceneManager");
			static int32_t sceneNum = 0;
			const char* scenes[] = { "RhythmGameTest", "MainTest", "ControllerTest", "CollidersTest", "SoundTest" };
			ImGui::Combo("##SceneNumCombo", &sceneNum, scenes, IM_ARRAYSIZE(scenes));
			ImGui::SameLine();
			if (ImGui::Button("Go!!!")) {
				if (!SceneManager::IsSceneChanging()) {
					switch (sceneNum) {
					case 0:
						SceneManager::Change<GameScene, SimpleSceneTransition>();
						break;
					case 1:
						SceneManager::Change<MainTestScene, SimpleSceneTransition>();
						break;
					case 2:
						SceneManager::Change<ControllerScene, SimpleSceneTransition>();
						break;
					case 3:
						SceneManager::Change<CollidersScene, SimpleSceneTransition>();
						break;
					case 4:
						SceneManager::Change<SoundScene, SimpleSceneTransition>();
						break;
					}
				}
			}

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::Text("DebugCamera Control");
			ImGui::Text("* Valid only in available scenes.");
			ImGui::Text("Left Control Key : Enable / Disable");
			ImGui::Text("WASD Key : Move");
			ImGui::Text("Space Key : Up");
			ImGui::Text("LShift Key : Down");
			ImGui::Text("Mouse : Direction");
			ImGui::End();
		}

		RImGui::Render();

		RDirectX::PostDraw();
		bufferLockInDrawing.unlock();

		TimeManager::Update();

		if (RInput::GetKey(DIK_ESCAPE)) {
			break;
		}
	}

	SceneManager::Finalize();
	RAudio::Final();
	RImGui::Finalize();

	return 0;
}