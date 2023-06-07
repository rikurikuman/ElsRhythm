#pragma once
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

class RImGui
{
public:
	static RImGui* GetInstance();

	//初期化処理まとめ
	static void Init();

	//フレーム開始時処理
	static void NewFrame();

	//描画処理まとめ
	static void Render();

	//終了処理まとめ
	static void Finalize();

private:
	ID3D12Device* device = nullptr;
	ID3D12DescriptorHeap* srvHeap = nullptr;

	void InitInternal();
	void FinalInternal();

	RImGui() {};
	~RImGui() = default;
	RImGui(const RImGui& a) = delete;
	RImGui& operator=(const RImGui&) = delete;
	RImGui(const RImGui&& a) = delete;
	RImGui& operator=(const RImGui&&) = delete;
};

