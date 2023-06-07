#include "Texture.h"
#include "RDirectX.h"
#include <DirectXTex.h>
#include "Color.h"
#include "Util.h"

using namespace std;
using namespace DirectX;

void TextureManager::Init()
{
	HRESULT result;

	//デスクリプタヒープ(SRV)
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	srvHeapDesc.NumDescriptors = numSRVDescritors;

	//生成
	srvHeap = nullptr;
	result = RDirectX::GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	RegisterInternal(GetEmptyTexture(), "PreRegisteredTex_Empty");
	RegisterInternal(GetHogeHogeTexture(), "PreRegisteredTex_HogeHoge");
}

Texture TextureManager::GetEmptyTexture()
{
	HRESULT result;

	Texture texture = Texture();

	const size_t textureWidth = 100;
	const size_t textureHeight = 100;
	const size_t imageDataCount = textureWidth * textureHeight;
	Color* imageData = new Color[imageDataCount];

	for (size_t i = 0; i < imageDataCount; i++) {
		size_t x = i % textureWidth;
		size_t y = i / textureWidth;

		imageData[i] = Color(1, 1, 1, 1);
	}

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);
	assert(SUCCEEDED(result));

	result = texture.resource->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(Color) * textureWidth,
		sizeof(Color) * imageDataCount
	);

	delete[] imageData;
	return texture;
}

Texture TextureManager::GetHogeHogeTexture()
{
	HRESULT result;

	Texture texture = Texture();

	const size_t textureWidth = 256;
	const size_t textureHeight = 256;
	const size_t imageDataCount = textureWidth * textureHeight;
	Color* imageData = new Color[imageDataCount];

	for (size_t i = 0; i < imageDataCount; i++) {
		size_t x = i % textureWidth;
		size_t y = i / textureWidth;
		
		if ((x < textureWidth / 2 && y < textureHeight / 2)
			|| (x >= textureWidth / 2 && y >= textureHeight / 2)) {
			imageData[i] = Color(1, 0, 1, 1);
		}
		else {
			imageData[i] = Color(0, 0, 0, 1);
		}
	}

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);
	assert(SUCCEEDED(result));

	result = texture.resource->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(Color) * textureWidth,
		sizeof(Color) * imageDataCount
	);

	delete[] imageData;
	return texture;
}

TextureHandle TextureManager::CreateInternal(const Color color, const UINT64 width, const UINT height, const std::string handle)
{
	std::lock_guard<std::recursive_mutex> lock(mutex);
	HRESULT result;

	Texture texture = Texture();

	const size_t imageDataCount = width * height;
	Color* imageData = new Color[imageDataCount];

	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i] = color;
	}

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = width;
	textureResourceDesc.Height = height;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);
	assert(SUCCEEDED(result));

	result = texture.resource->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(Color) * static_cast<UINT>(width),
		sizeof(Color) * static_cast<UINT>(imageDataCount)
	);

	delete[] imageData;
	return RegisterInternal(texture, handle);
}

TextureHandle TextureManager::CreateInternal(const Color* pSource, const UINT64 width, const UINT height, const std::string filepath, const std::string handle)
{
	std::lock_guard<std::recursive_mutex> lock(mutex);
	HRESULT result;

	if (!filepath.empty()) {
		//一回読み込んだことがあるファイルはそのまま返す
		auto itr = find_if(textureMap.begin(), textureMap.end(), [&](const std::pair<TextureHandle, Texture>& p) {
			return p.second.filePath == filepath;
			});
		if (itr != textureMap.end()) {
			return itr->first;
		}
	}

	Texture texture = Texture();
	texture.filePath = filepath;

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	textureResourceDesc.Width = width;
	textureResourceDesc.Height = height;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	//てんそー
	result = texture.resource->WriteToSubresource(
		0,
		nullptr, //全領域へコピー
		pSource, //元データアドレス
		static_cast<UINT>(sizeof(Color) * width), //1ラインサイズ
		static_cast<UINT>(sizeof(Color) * width * static_cast<UINT64>(height)) //1枚サイズ
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	return RegisterInternal(texture, handle);
}

TextureHandle TextureManager::LoadInternal(const std::string filepath, const std::string handle)
{
	std::unique_lock<std::recursive_mutex> lock(mutex);
	HRESULT result;

	//一回読み込んだことがあるファイルはそのまま返す
	auto itr = find_if(textureMap.begin(), textureMap.end(), [&](const std::pair<TextureHandle, Texture>& p) {
		return p.second.filePath == filepath;
		});
	if (itr != textureMap.end()) {
		return itr->first;
	}
	lock.unlock();

	Texture texture = Texture();
	texture.filePath = filepath;
	wstring wfilePath(filepath.begin(), filepath.end());

	// 画像イメージデータ
	TexMetadata imgMetadata{};
	ScratchImage scratchImg{};
	// WICテクスチャのロード
	result = LoadFromWICFile(
		wfilePath.c_str(),
		WIC_FLAGS_NONE,
		&imgMetadata, scratchImg
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		imgMetadata = scratchImg.GetMetadata();
	}
	else {
		return "FailedTextureHandle";
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	imgMetadata.format = MakeSRGB(imgMetadata.format);

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = imgMetadata.format;
	textureResourceDesc.Width = imgMetadata.width;
	textureResourceDesc.Height = (UINT)imgMetadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)imgMetadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)imgMetadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	//てんそー
	//全ミップマップについて
	for (size_t i = 0; i < imgMetadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture.resource->WriteToSubresource(
			(UINT)i,
			nullptr, //全領域へコピー
			img->pixels, //元データアドレス
			(UINT)img->rowPitch, //1ラインサイズ
			(UINT)img->slicePitch //1枚サイズ
		);
		if (FAILED(result)) {
			return "FailedTextureHandle";
		}
	}

	return RegisterInternal(texture, handle);
}

TextureHandle TextureManager::LoadInternal(const void* pSource, const size_t size, const std::string filepath, const std::string handle)
{
	std::unique_lock<std::recursive_mutex> lock(mutex);
	HRESULT result;

	if (!filepath.empty()) {
		//一回読み込んだことがあるファイルはそのまま返す
		auto itr = find_if(textureMap.begin(), textureMap.end(), [&](const std::pair<TextureHandle, Texture>& p) {
			return p.second.filePath == filepath;
			});
		if (itr != textureMap.end()) {
			return itr->first;
		}
	}
	lock.unlock();

	Texture texture = Texture();
	texture.filePath = filepath;

	// 画像イメージデータ
	TexMetadata imgMetadata{};
	ScratchImage scratchImg{};
	// WICテクスチャのロード
	result = LoadFromWICMemory(
		pSource,
		size,
		WIC_FLAGS_NONE,
		&imgMetadata, scratchImg
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		imgMetadata = scratchImg.GetMetadata();
	}
	else {
		return "FailedTextureHandle";
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	imgMetadata.format = MakeSRGB(imgMetadata.format);

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = imgMetadata.format;
	textureResourceDesc.Width = imgMetadata.width;
	textureResourceDesc.Height = (UINT)imgMetadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)imgMetadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)imgMetadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource)
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	//てんそー
	//全ミップマップについて
	for (size_t i = 0; i < imgMetadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture.resource->WriteToSubresource(
			(UINT)i,
			nullptr, //全領域へコピー
			img->pixels, //元データアドレス
			(UINT)img->rowPitch, //1ラインサイズ
			(UINT)img->slicePitch //1枚サイズ
		);
		if (FAILED(result)) {
			return "FailedTextureHandle";
		}
	}

	return RegisterInternal(texture, handle);
}

Texture& TextureManager::GetInternal(const TextureHandle& handle)
{
	std::lock_guard<std::recursive_mutex> lock(mutex);
	if (handle.empty()) {
		return textureMap["PreRegisteredTex_Empty"];
	}

	auto itr = textureMap.find(handle);
	if (itr != textureMap.end()) {
		return itr->second;
	}
	
	return textureMap["PreRegisteredTex_HogeHoge"];
}

TextureHandle TextureManager::RegisterInternal(Texture texture, TextureHandle handle)
{
	std::unique_lock<std::recursive_mutex> lock(mutex);
	UINT useIndex = -1; 

	auto itr = textureMap.find(handle);
	if (itr != textureMap.end()) {
		useIndex = itr->second.heapIndex;
	}
	else {
		for (UINT i = 0; i < TextureManager::numSRVDescritors; i++) {
			bool ok = true;
			for (std::pair<const TextureHandle, Texture>& p : textureMap) {
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
	lock.unlock();

	if (useIndex == -1) {
		//over
		return TextureHandle();
	}

	//シェーダーリソースビュー
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	size_t incrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_cpuHandle.ptr += useIndex * incrementSize;
	_gpuHandle.ptr += useIndex * incrementSize;
	texture.cpuHandle = _cpuHandle;
	texture.gpuHandle = _gpuHandle;
	texture.heapIndex = useIndex;

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture.resource->GetDesc().Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texture.resource->GetDesc().MipLevels;

	//生成
	RDirectX::GetDevice()->CreateShaderResourceView(texture.resource.Get(), &srvDesc, _cpuHandle);

	if (handle.empty()) {
		handle = "NoNameHandle_" + to_string(useIndex);
	}

	lock.lock();
	textureMap[handle] = texture;
	return handle;
}

void TextureManager::UnRegisterInternal(const TextureHandle& handle)
{
	std::lock_guard<std::recursive_mutex> lock(mutex);
	textureMap.erase(handle);
}

void TextureManager::UnRegisterAtEndFrameInternal(const TextureHandle& handle)
{
	std::lock_guard<std::recursive_mutex> lock(mutex);
	unregisterScheduledList.push_back(handle);
}

void TextureManager::EndFrameProcessInternal()
{
	std::lock_guard<std::recursive_mutex> lock(mutex);
	for (TextureHandle& handle : unregisterScheduledList) {
		UnRegisterInternal(handle);
	}

	unregisterScheduledList.clear();
}

TextureHandle TextureManager::Create(const Color color, const UINT64 width, const UINT height, const std::string handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->CreateInternal(color, width, height, handle);
}

TextureHandle TextureManager::Create(const Color* pSource, const UINT64 width, const UINT height, const std::string filepath, const std::string handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->CreateInternal(pSource, width, height, filepath, handle);
}

TextureHandle TextureManager::Load(const string filepath, const std::string handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->LoadInternal(filepath, handle);
}

TextureHandle TextureManager::Load(const void* pSource, const size_t size, const std::string filepath, const std::string handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->LoadInternal(pSource, size, filepath, handle);
}

Texture& TextureManager::Get(const TextureHandle& handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->GetInternal(handle);
}

TextureHandle TextureManager::Register(Texture texture, TextureHandle handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->RegisterInternal(texture, handle);
}

void TextureManager::UnRegister(const TextureHandle& handle) {
	TextureManager* manager = TextureManager::GetInstance();
	manager->UnRegisterInternal(handle);
}

void TextureManager::UnRegisterAtEndFrame(const TextureHandle& handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	manager->UnRegisterAtEndFrameInternal(handle);
}

void TextureManager::EndFrameProcess()
{
	TextureManager* manager = TextureManager::GetInstance();
	manager->EndFrameProcessInternal();
}

void TextureManager::UnRegisterAll()
{
	TextureManager* manager = TextureManager::GetInstance();
	manager->textureMap.clear();
}