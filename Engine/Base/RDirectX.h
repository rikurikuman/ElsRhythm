#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#include "Texture.h"
#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Color.h"

using namespace Microsoft::WRL;

class RDirectX
{
private:
	ComPtr<ID3D12Device> mDevice = nullptr;
	ComPtr<IDXGIFactory7> mDxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> mSwapChain = nullptr;
	ComPtr<ID3D12CommandAllocator> mCmdAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> mCmdList = nullptr;
	ComPtr<ID3D12CommandQueue> mCmdQueue = nullptr;
	ComPtr<ID3D12DescriptorHeap> mRtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> mSrvHeap = nullptr;
	std::vector<ComPtr<ID3D12Resource>> mBackBuffers;
	ComPtr<ID3D12Resource> mDepthBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap;
	ComPtr<ID3D12Fence> mFence = nullptr;
	size_t mFenceVal = 0;
	Shader mBasicVS;
	Shader mBasicPS;
	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;

public:
	//DirectX������
	static void Init();
	static RDirectX* GetInstance();

	//�f�o�C�X���擾����
	static ID3D12Device* GetDevice();

	//�R�}���h���X�g���擾����
	static ID3D12GraphicsCommandList* GetCommandList();

	//�ق�ImGui��p�݂����ɂȂ��Ă�mSrvHeap���v���[���g
	static ID3D12DescriptorHeap* GetSRVHeap();

	//��{���[�g�V�O�l�`�����擾����
	static RootSignature GetDefRootSignature();

	//��{�p�C�v���C�����擾����
	static GraphicsPipeline GetDefPipeline();

	//�����ǂ������ɕ`��O����������
	static void PreDraw();

	//�����ǂ������ɕ`��㏈��������
	static void PostDraw();

	//�o�b�N�o�b�t�@�̐����擾����
	static size_t GetBackBufferSize();

	//���݂̃o�b�N�o�b�t�@�ԍ����擾����
	static UINT GetCurrentBackBufferIndex();

	//���݂̃o�b�N�o�b�t�@�̃��\�[�X���擾����
	static ID3D12Resource* GetCurrentBackBufferResource();

	//���݂̃o�b�N�o�b�t�@��RTV�n���h�����擾����
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle();

	//�o�b�N�o�b�t�@��DSV�n���h�����擾����
	static D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferDSVHandle();

	//���\�[�X�o���A��PRESENT->RENDER_TARGET�ɂ��鏈��
	static void OpenResorceBarrier(ID3D12Resource* mResource);

	//���\�[�X�o���A��RENDER_TARGET->PRESENT�ɂ��鏈��
	static void CloseResourceBarrier(ID3D12Resource* mResource);

	//�o�b�N�o�b�t�@���N���A����
	static void ClearBackBuffer(Color color);

	//�R�}���h���X�g����Ď��s���A�t���b�v���āA�R�}���h���X�g���ĂъJ����
	static void RunDraw();

private:
	RDirectX() {};
	~RDirectX() = default;
	RDirectX(const RDirectX& a) = delete;
	RDirectX& operator=(const RDirectX&) = delete;

	D3D12_CPU_DESCRIPTOR_HANDLE mNowRtvHandle = {};

	void InitInternal();
};