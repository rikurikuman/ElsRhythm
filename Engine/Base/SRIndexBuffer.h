#pragma once
#include <vector>
#include <SRBuffer.h>

class SRIndexBuffer {
private:
	class IndexBufferData {
	public:
		size_t count = 0;
		UINT dataSize = 0;
		UINT indexCount = 0;
		SRBufferPtr buff;
	};

	static std::recursive_mutex mutex;

public:
	SRIndexBuffer() {};

	~SRIndexBuffer() {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
		std::lock_guard<std::recursive_mutex> lock2(mutex);
		if (data != nullptr) {
			data->count--;
			if (data->count == 0) {
				SRBufferAllocator::Free(data->buff);
				delete data;
			}
		}
	}

	//ÉRÉsÅ[ëŒçÙ
	SRIndexBuffer(const SRIndexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (data != nullptr) {
			data->count--;
			if (data->count == 0) {
				SRBufferAllocator::Free(data->buff);
				delete data;
			}
		}
		data = o.data;
		if (data != nullptr) data->count++;
	}

	SRIndexBuffer& operator=(const SRIndexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (this != &o) {
			if (data != nullptr) {
				data->count--;
				if (data->count == 0) {
					SRBufferAllocator::Free(data->buff);
					delete data;
				}
			}
			data = o.data;
			if (data != nullptr) data->count++;
		}
		return *this;
	}

	SRIndexBuffer(UINT* list, UINT size);
	SRIndexBuffer(std::vector<UINT> list);

	void Init(UINT* list, UINT size);
	void Init(std::vector<UINT> list);

	bool IsValid() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return this->data != nullptr && this->data->buff.GetRegionPtr() != nullptr && this->data->buff.GetRegionPtr()->region != nullptr;
	}

	operator bool() {
		return IsValid();
	}

	D3D12_INDEX_BUFFER_VIEW GetIndexView();
	UINT GetIndexCount();

private:
	IndexBufferData* data = nullptr;
};