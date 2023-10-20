//外部ヘッダ
#pragma warning(push, 0)
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include <Windows.h>
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include <string>
#pragma warning(pop)

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
#include "TitleScene.h"
#include "Colliders.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <TestRenderStage.h>
#include <SRBuffer.h>
#include <ParticleObject.h>
#include "EventSystem.h"
#include <DebugGUI.h>
#include <ParticleManager.h>

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
	RWindow::SetWindowName(L"リズムマウサー");
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

	SceneManager::Set<TitleScene>();

	ParticleObject::Clear();

	DebugCamera camera({ 0, 0, -10 });

	TimeManager::targetFPS = 60;

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
		ParticleManager::Update();

		//以下描画処理
		RDirectX::PreDraw();
		Renderer::SetAllParamaterToAuto();

		//画面クリア～
		RDirectX::ClearBackBuffer(Color(0.0f, 0.0f, 0.0f, 1.0f));

		//描画コマンド
		SceneManager::Draw();
		ParticleManager::Draw();
		SimpleDrawer::DrawAll();

		//べんり！
		if (Util::debugBool) {
			DebugGUI::Show();
		}

		//描画の実行中に別スレッドとかでSRBufferとかの操作をされるとぶっ飛ぶので雑にロックする
		//いずれもっと良い制御にしたい
		std::unique_lock<std::recursive_mutex> bufferLockInDrawing(SRBufferAllocator::GetInstance()->sMutex);
		Renderer::Execute();

		RImGui::Render();

		RDirectX::PostDraw();
		bufferLockInDrawing.unlock();

		EventSystem::Clear();
		TimeManager::Update();
	}

	SceneManager::Finalize();
	RAudio::Final();
	RImGui::Finalize();

	return 0;
}