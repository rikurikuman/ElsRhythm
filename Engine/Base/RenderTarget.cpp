#include "RenderTarget.h"
#include "RDirectX.h"

void RenderTarget::SetToBackBuffer()
{
	RenderTarget* manager = GetInstance();

	for (std::string name : manager->currentRenderTargets) {
		if (name.empty()) {
			RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());
			continue;
		}

		RenderTargetTexture* tex = manager->GetRenderTargetTexture(name);
		if (tex != nullptr) tex->CloseResourceBarrier();
	}
	manager->currentRenderTargets.clear();

	RDirectX::OpenResorceBarrier(RDirectX::GetCurrentBackBufferResource());
	RDirectX::GetCommandList()->OMSetRenderTargets(1, &RDirectX::GetCurrentBackBufferHandle(), false, &RDirectX::GetBackBufferDSVHandle());
	manager->currentRenderTargets.push_back("");
}

void RenderTarget::SetToTexture(std::string name)
{
	RenderTarget* manager = GetInstance();

	for (std::string name : manager->currentRenderTargets) {
		if (name.empty()) {
			RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());
			continue;
		}

		RenderTargetTexture* tex = manager->GetRenderTargetTexture(name);
		if (tex != nullptr) tex->CloseResourceBarrier();
	}
	manager->currentRenderTargets.clear();

	RenderTargetTexture* tex = manager->GetRenderTargetTexture(name);

	if (tex == nullptr) {
		//ないよ
#ifdef _DEBUG
		OutputDebugStringA(("RKEngine ERROR: Failed SetToTexture. RenderTargetTexture(" + name + ") isn't exist.\n").c_str());
#endif
		return;
	}

	tex->OpenResourceBarrier();
	RDirectX::GetCommandList()->OMSetRenderTargets(1, &tex->GetRTVHandle(), false, &tex->GetDSVHandle());
	manager->currentRenderTargets.push_back(name);
}

void RenderTarget::SetToTexture(std::vector<std::string> names)
{
	RenderTarget* manager = GetInstance();

	for (std::string name : manager->currentRenderTargets) {
		if (name.empty()) {
			RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());
			continue;
		}

		RenderTargetTexture* tex = manager->GetRenderTargetTexture(name);
		if (tex != nullptr) tex->CloseResourceBarrier();
	}
	manager->currentRenderTargets.clear();

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandles;

	for (std::string& name : names) {
		RenderTargetTexture* tex = manager->GetRenderTargetTexture(name);

		if (tex == nullptr) {
			//ないよ
#ifdef _DEBUG
			OutputDebugStringA(("RKEngine ERROR: Failed SetToTexture. RenderTargetTexture(" + name + ") isn't exist.\n").c_str());
#endif
			return;
		}

		tex->OpenResourceBarrier();
		rtvHandles.push_back(tex->GetRTVHandle());
		dsvHandles.push_back(tex->GetDSVHandle());
		manager->currentRenderTargets.push_back(name);
	}

	RDirectX::GetCommandList()->OMSetRenderTargets((UINT)names.size(), &rtvHandles[0], false, &dsvHandles[0]);
}

void RenderTarget::CreateRenderTargetTexture(const UINT width, const UINT height, const Color clearColor, TextureHandle name)
{
	RenderTarget* manager = GetInstance();

	HRESULT result;

	RenderTargetTexture renderTarget;
	Texture texture = Texture();

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureResourceDesc.Width = width;
	textureResourceDesc.Height = height;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;
	textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12_CLEAR_VALUE textureClearValue{};
	textureClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureClearValue.Color[0] = clearColor.r;
	textureClearValue.Color[1] = clearColor.g;
	textureClearValue.Color[2] = clearColor.b;
	textureClearValue.Color[3] = clearColor.a;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&textureClearValue,
		IID_PPV_ARGS(&texture.resource)
	);
	assert(SUCCEEDED(result));

	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = width;
	depthResDesc.Height = height;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //デプスステンシル

	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	result = RDirectX::GetDevice()->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&renderTarget.depthBuff)
	);
	assert(SUCCEEDED(result));

	TextureHandle texHandle = "RenderTargetTex_" + name;
	TextureManager::Register(texture, texHandle);

	renderTarget.name = name;
	renderTarget.texHandle = texHandle;
	renderTarget.clearColor = clearColor;

	UINT useIndex = -1;

	auto itr = manager->renderTargetMap.find(name);
	if (itr != manager->renderTargetMap.end()) {
		useIndex = itr->second.heapIndex;
	}
	else {
		for (UINT i = 0; i < numDescriptors; i++) {
			bool ok = true;
			for (std::pair<const std::string, RenderTargetTexture>& p : manager->renderTargetMap) {
				if (p.second.heapIndex == i) {
					ok = false;
					break;
				}
			}

			if (ok) {
				useIndex = i;
				break;
			}
		}
	}

	if (useIndex == -1) {
		//Over.
		return;
	}

	renderTarget.heapIndex = useIndex;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	size_t rtvincrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = manager->rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHeapHandle.ptr += useIndex * rtvincrementSize;
	RDirectX::GetDevice()->CreateRenderTargetView(texture.resource.Get(), &rtvDesc, rtvHeapHandle);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	size_t dsvincrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle = manager->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	dsvHeapHandle.ptr += useIndex * dsvincrementSize;
	RDirectX::GetDevice()->CreateRenderTargetView(texture.resource.Get(), &rtvDesc, rtvHeapHandle);

	RDirectX::GetDevice()->CreateDepthStencilView(renderTarget.depthBuff.Get(), &dsvDesc, dsvHeapHandle);

	manager->renderTargetMap[name] = renderTarget;
}

RenderTargetTexture* RenderTarget::GetRenderTargetTexture(std::string name) {
	RenderTarget* manager = GetInstance();
	auto itr = manager->renderTargetMap.find(name);
	if (itr != manager->renderTargetMap.end()) {
		return &itr->second;
	}

	//ないよ
	return nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRTVHandle(UINT index)
{
	RenderTarget* manager = RenderTarget::GetInstance();

	size_t rtvincrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = manager->rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHeapHandle.ptr += index * rtvincrementSize;
	return rtvHeapHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetDSVHandle(UINT index)
{
	RenderTarget* manager = RenderTarget::GetInstance();

	size_t dsvincrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle = manager->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	dsvHeapHandle.ptr += index * dsvincrementSize;
	return dsvHeapHandle;
}

void RenderTarget::CreateHeaps()
{
	HRESULT result;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = numDescriptors;

	result = RDirectX::GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = numDescriptors;

	result = RDirectX::GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	assert(SUCCEEDED(result));
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetTexture::GetRTVHandle()
{
	return RenderTarget::GetRTVHandle(heapIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetTexture::GetDSVHandle()
{
	return RenderTarget::GetDSVHandle(heapIndex);
}

void RenderTargetTexture::OpenResourceBarrier()
{
	if (!isBarrier) {
		//もう開いてる！！！
#ifdef _DEBUG
		OutputDebugStringA(("RKEngine WARNING: Tried to open a ResourceBarrier of RenderTargetTexture(" + name + ") that is already open.\n").c_str());
#endif
		return;
	}

	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = TextureManager::Get(texHandle).resource.Get();
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	RDirectX::GetCommandList()->ResourceBarrier(1, &barrierDesc);
	isBarrier = false;
}

void RenderTargetTexture::CloseResourceBarrier()
{
	if (isBarrier) {
		//もう閉じてる！！！
#ifdef _DEBUG
		OutputDebugStringA(("RKEngine WARNING: Tried to close a ResourceBarrier of RenderTargetTexture(" + name + ") that is already close.\n").c_str());
#endif
		return;
	}

	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = TextureManager::Get(texHandle).resource.Get();
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	RDirectX::GetCommandList()->ResourceBarrier(1, &barrierDesc);
	isBarrier = true;
}

void RenderTargetTexture::ClearRenderTarget()
{
	bool barrier = isBarrier;

	if (barrier) OpenResourceBarrier();
	FLOAT color[] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
	RDirectX::GetCommandList()->ClearRenderTargetView(GetRTVHandle(), color, 0, nullptr);
	if (barrier) CloseResourceBarrier();
}

void RenderTargetTexture::ClearDepthStencil()
{
	bool barrier = isBarrier;

	if (barrier) OpenResourceBarrier();
	RDirectX::GetCommandList()->ClearDepthStencilView(
		GetDSVHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
	if (barrier) CloseResourceBarrier();
}
