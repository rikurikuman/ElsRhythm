#include "SRVertexBuffer.h"

std::recursive_mutex SRVertexBuffer::mutex;

SRVertexBuffer::SRVertexBuffer(VertexP* list, unsigned int size)
{
	Init(list, size);
}

SRVertexBuffer::SRVertexBuffer(std::vector<VertexP> list)
{
	Init(list);
}

SRVertexBuffer::SRVertexBuffer(VertexPNU* list, unsigned int size)
{
	Init(list, size);
}

SRVertexBuffer::SRVertexBuffer(std::vector<VertexPNU> list)
{
	Init(list);
}

void SRVertexBuffer::Init(VertexP* list, unsigned int size)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);

	if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(data->buff);
	}
	else {
		data = new VertexBufferData(); //できればnewしたくねえ
		data->count++;
	}

	UINT dataSize = static_cast<UINT>(sizeof(VertexP) * size);

	data->buff = SRBufferAllocator::Alloc(dataSize, 1);

	VertexP* vertMap = reinterpret_cast<VertexP*>(data->buff.Get());
	for (UINT i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->strideInBytes = sizeof(VertexP);
}

void SRVertexBuffer::Init(std::vector<VertexP> list)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);

	if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(data->buff);
	}
	else {
		data = new VertexBufferData(); //できればnewしたくねえ
		data->count++;
	}

	UINT dataSize = static_cast<UINT>(sizeof(VertexP) * list.size());

	data->buff = SRBufferAllocator::Alloc(dataSize, 1);

	VertexP* vertMap = reinterpret_cast<VertexP*>(data->buff.Get());
	for (UINT i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->strideInBytes = sizeof(VertexP);
}

void SRVertexBuffer::Init(VertexPNU* list, unsigned int size)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);

	if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(data->buff);
	}
	else {
		data = new VertexBufferData(); //できればnewしたくねえ
		data->count++;
	}

	UINT dataSize = static_cast<UINT>(sizeof(VertexPNU) * size);

	data->buff = SRBufferAllocator::Alloc(dataSize, 1);

	VertexPNU* vertMap = reinterpret_cast<VertexPNU*>(data->buff.Get());
	for (UINT i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->strideInBytes = sizeof(VertexPNU);
}

void SRVertexBuffer::Init(std::vector<VertexPNU> list)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);

	if (data != nullptr && data->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(data->buff);
	}
	else {
		data = new VertexBufferData(); //できればnewしたくねえ
		data->count++;
	}

	UINT dataSize = static_cast<UINT>(sizeof(VertexPNU) * list.size());

	data->buff = SRBufferAllocator::Alloc(dataSize, 1);

	VertexPNU* vertMap = reinterpret_cast<VertexPNU*>(data->buff.Get());
	for (UINT i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->strideInBytes = sizeof(VertexPNU);
}

void SRVertexBuffer::Update(VertexPNU* list, unsigned int size)
{
	std::lock_guard<std::recursive_mutex> lock(mutex);
	if (data == nullptr || data->buff.GetRegionPtr() == nullptr) {
		Init(list, size);
		return;
	}

	UINT dataSize = static_cast<UINT>(sizeof(VertexPNU) * size);

	if (data->dataSize != dataSize || data->strideInBytes != sizeof(VertexPNU)) {
		Init(list, size);
		return;
	}

	VertexPNU* vertMap = reinterpret_cast<VertexPNU*>(data->buff.Get());
	for (UINT i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	data->dataSize = dataSize;
	data->strideInBytes = sizeof(VertexPNU);
}

D3D12_VERTEX_BUFFER_VIEW SRVertexBuffer::GetVertView()
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->mutex);
	std::lock_guard<std::recursive_mutex> lock2(mutex);
	if (data == nullptr || data->buff.GetRegionPtr() == nullptr) {
		return D3D12_VERTEX_BUFFER_VIEW();
	}
	D3D12_VERTEX_BUFFER_VIEW view{};
	view.BufferLocation = data->buff.GetGPUVirtualAddress();
	view.SizeInBytes = data->dataSize; //頂点バッファのサイズ
	view.StrideInBytes = data->strideInBytes; //頂点一個のサイズ
	return view;
}