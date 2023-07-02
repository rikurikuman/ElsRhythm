#pragma once
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

class RImGui
{
public:
	static RImGui* GetInstance();

	//�����������܂Ƃ�
	static void Init();

	//�t���[���J�n������
	static void NewFrame();

	//�`�揈���܂Ƃ�
	static void Render();

	//�I�������܂Ƃ�
	static void Finalize();

private:
	ID3D12Device* mDevicePtr = nullptr;
	ID3D12DescriptorHeap* mSrvHeapPtr = nullptr;

	void InitInternal();
	void FinalInternal();

	RImGui() {};
	~RImGui() = default;
	RImGui(const RImGui& a) = delete;
	RImGui& operator=(const RImGui&) = delete;
	RImGui(const RImGui&& a) = delete;
	RImGui& operator=(const RImGui&&) = delete;
};

