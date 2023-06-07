#pragma once
#include "Texture.h"
#include <vector>

class RenderTargetTexture
{
public:
	std::string name;
	TextureHandle texHandle;
	Color clearColor;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff;
	UINT heapIndex = -1;

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle();

	void OpenResourceBarrier();
	void CloseResourceBarrier();
	void ClearRenderTarget();
	void ClearDepthStencil();

private:
	bool isBarrier = true;
};

class RenderTarget
{
public:
	static void SetToBackBuffer();
	static void SetToTexture(std::string name);
	static void SetToTexture(std::vector<std::string> names);

	static void CreateRenderTargetTexture(const UINT width, const UINT height, const Color clearColor, TextureHandle name);
	static RenderTargetTexture* GetRenderTargetTexture(std::string name);
	static D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(UINT index);
	static D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(UINT index);

	static RenderTarget* GetInstance() {
		static RenderTarget instance;
		return &instance;
	}
private:
	RenderTarget() {
		CreateHeaps();
	};
	~RenderTarget() = default;
	RenderTarget(const RenderTarget& a) {};
	RenderTarget& operator=(const RenderTarget&) { return *this; }

	void CreateHeaps();

	static const UINT numDescriptors = 256;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	std::vector<std::string> currentRenderTargets;
	std::map<std::string, RenderTargetTexture> renderTargetMap;
};

