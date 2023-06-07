#include "SRIndexBuffer.h"

std::recursive_mutex SRIndexBuffer::mutex;

SRIndexBuffer::SRIndexBuffer(UINT* list, UINT size)
{
	Init(list, size);
}

SRIndexBuffer::SRIndexBuffer(std::vector<UINT> list)
{
	Init(list);
}

void SRIndexBuffer::Init(UINT* list, UINT size)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);

	if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(data->buff);
	}
	else {
		data = new IndexBufferData(); //できればnewしたくねえ
		data->count++;
	}

	UINT dataSize = static_cast<UINT>(sizeof(UINT) * size);

	data->buff = SRBufferAllocator::Alloc(dataSize, 1);

	UINT* vertMap = reinterpret_cast<UINT*>(data->buff.Get());
	for (UINT i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->indexCount = size;
}

void SRIndexBuffer::Init(std::vector<UINT> list)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);

	if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(data->buff);
	}
	else {
		data = new IndexBufferData(); //できればnewしたくねえ
		data->count++;
	}

	UINT dataSize = static_cast<UINT>(sizeof(UINT) * list.size());

	data->buff = SRBufferAllocator::Alloc(dataSize, 1);

	UINT* vertMap = reinterpret_cast<UINT*>(data->buff.Get());
	for (UINT i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->indexCount = static_cast<UINT>(list.size());
}

D3D12_INDEX_BUFFER_VIEW SRIndexBuffer::GetIndexView()
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);
	if (data == nullptr || data->buff.GetRegionPtr() == nullptr) {
		return D3D12_INDEX_BUFFER_VIEW();
	}
	D3D12_INDEX_BUFFER_VIEW view{};
	view.BufferLocation = data->buff.GetGPUVirtualAddress();
	view.SizeInBytes = data->dataSize; //頂点バッファのサイズ
	view.Format = DXGI_FORMAT_R32_UINT;
	return view;
}

UINT SRIndexBuffer::GetIndexCount()
{
	std::lock_guard<std::recursive_mutex> lock2(mutex);
	if (data == nullptr) {
		return 0;
	}
	return data->indexCount;
}
