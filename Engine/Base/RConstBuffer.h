#pragma once
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include "RDirectX.h"
#include "Util.h"
#include <chrono>
#include <mutex>

template <typename T>
class RConstBuffer
{
private:
	static std::recursive_mutex mutex;
	static std::map<void*, size_t> countMap;

	static void AddCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		countMap[p]++;

#ifdef _RCONSTBUFFER_DEBUG_
		OutputDebugString((std::wstring(L"RConstBuffer(") + Util::ConvertStringToWString(Util::StringFormat("%p", p)) + L") Add      : " + std::to_wstring(countMap[p]) + L"\n").c_str());
#endif
	}

	static void SubtractCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (countMap[p] == 0) {
			int hoge = 0;
		}
		countMap[p]--;

#ifdef _RCONSTBUFFER_DEBUG_
		OutputDebugString((std::wstring(L"RConstBuffer(") + Util::ConvertStringToWString(Util::StringFormat("%p", p)) + L") Subtract : " + std::to_wstring(countMap[p]) + L"\n").c_str());
#endif

		if (countMap[p] == 0) {
			countMap.erase(p);
		}
	}

	static size_t GetCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		auto itr = countMap.find(p);
		if (itr == countMap.end()) {
			return 0;
		}
		return countMap[p];
	}

	void Init() {
		HRESULT result;

		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		// リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(T) + 0xff) & ~0xff; //256バイトアラインメント
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		
		// 定数バッファの生成
		result = RDirectX::GetDevice()->CreateCommittedResource(
			&cbHeapProp, //ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc, //リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff)
		);
		assert(SUCCEEDED(result));
	}

public:
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff = nullptr;
	T* constMap = nullptr;

	RConstBuffer() {
		Init();
		Map();
		RConstBuffer::AddCount(constBuff.Get());
	}
	RConstBuffer(const RConstBuffer& o) {
		constBuff = o.constBuff;
		constMap = o.constMap;
		RConstBuffer::AddCount(constBuff.Get());
	}
	~RConstBuffer() {
		if (constBuff != nullptr) {
			RConstBuffer::SubtractCount(constBuff.Get());
			if (RConstBuffer::GetCount(constBuff.Get()) == 0) {
				UnMap();
			}
		}
	}
	
	RConstBuffer& operator=(const RConstBuffer& o) {
		if (this != &o) {
			if (constBuff != nullptr) {
				RConstBuffer::SubtractCount(constBuff.Get());
			}
			constBuff = o.constBuff;
			constMap = o.constMap;
			RConstBuffer::AddCount(constBuff.Get());
		}
		return *this;
	}

	void Map() {
		HRESULT result;
		result = constBuff->Map(0, nullptr, (void**)&constMap); //マッピング
		assert(SUCCEEDED(result));
	}

	void UnMap() {
		constBuff->Unmap(0, nullptr);
		constMap = nullptr;
	}
};

template<typename T>
std::map<void*, size_t> RConstBuffer<T>::countMap = std::map<void*, size_t>();

template<typename T>
std::recursive_mutex RConstBuffer<T>::mutex;