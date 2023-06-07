#include "VertexBuffer.h"
#include "RDirectX.h"

VertexBuffer::VertexBuffer(VertexP* list, unsigned int size)
{
	Init(list, size);
}

VertexBuffer::VertexBuffer(std::vector<VertexP> list)
{
	Init(list);
}

VertexBuffer::VertexBuffer(VertexPNU* list, unsigned int size)
{
	Init(list, size);
}

VertexBuffer::VertexBuffer(std::vector<VertexPNU> list)
{
	Init(list);
}

void VertexBuffer::Init(VertexP* list, unsigned int size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	UINT dataSize = static_cast<UINT>(sizeof(VertexP) * size);

	//���_�o�b�t�@���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//���_�o�b�t�@����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	//����͒��_�o�b�t�@�̃}�b�s���O
	VertexP* vertMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (UINT i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	view.BufferLocation = buff->GetGPUVirtualAddress(); //GPU���z�A�h���X
	view.SizeInBytes = dataSize; //���_�o�b�t�@�̃T�C�Y
	view.StrideInBytes = sizeof(VertexP); //���_��̃T�C�Y
}

void VertexBuffer::Init(std::vector<VertexP> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	UINT dataSize = static_cast<UINT>(sizeof(VertexP) * list.size());

	//���_�o�b�t�@���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//���_�o�b�t�@����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	//����͒��_�o�b�t�@�̃}�b�s���O
	VertexP* vertMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (UINT i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	view.BufferLocation = buff->GetGPUVirtualAddress(); //GPU���z�A�h���X
	view.SizeInBytes = dataSize; //���_�o�b�t�@�̃T�C�Y
	view.StrideInBytes = sizeof(VertexP); //���_��̃T�C�Y
}

void VertexBuffer::Init(VertexPNU* list, unsigned int size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	UINT dataSize = static_cast<UINT>(sizeof(VertexPNU) * size);

	//���_�o�b�t�@���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//���_�o�b�t�@����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	//����͒��_�o�b�t�@�̃}�b�s���O
	VertexPNU* vertMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (UINT i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	view.BufferLocation = buff->GetGPUVirtualAddress(); //GPU���z�A�h���X
	view.SizeInBytes = dataSize; //���_�o�b�t�@�̃T�C�Y
	view.StrideInBytes = sizeof(VertexPNU); //���_��̃T�C�Y
}

void VertexBuffer::Init(std::vector<VertexPNU> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	UINT dataSize = static_cast<UINT>(sizeof(VertexPNU) * list.size());

	//���_�o�b�t�@���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//���_�o�b�t�@����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	//����͒��_�o�b�t�@�̃}�b�s���O
	VertexPNU* vertMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (UINT i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	view.BufferLocation = buff->GetGPUVirtualAddress(); //GPU���z�A�h���X
	view.SizeInBytes = dataSize; //���_�o�b�t�@�̃T�C�Y
	view.StrideInBytes = sizeof(VertexPNU); //���_��̃T�C�Y
}

void VertexBuffer::Update(VertexPNU* list, unsigned int size)
{
	HRESULT result;

	UINT dataSize = static_cast<UINT>(sizeof(VertexPNU) * size);

	assert(buff->GetDesc().Width >= dataSize);
	if (buff->GetDesc().Width < dataSize) {
		return;
	}

	VertexPNU* vertMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (UINT i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	view.SizeInBytes = dataSize;
	view.StrideInBytes = sizeof(VertexPNU);
}
