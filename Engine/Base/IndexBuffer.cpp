#include "IndexBuffer.h"
#include "RDirectX.h"

IndexBuffer::IndexBuffer(unsigned int* list, unsigned int size)
{
	Init(list, size);
}

IndexBuffer::IndexBuffer(std::vector<unsigned int> list)
{
	Init(list);
}

void IndexBuffer::Init(unsigned int* list, unsigned int size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	UINT dataSize = static_cast<UINT>(sizeof(UINT) * size);

	//�C���f�b�N�X�o�b�t�@���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�C���f�b�N�X�o�b�t�@����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	UINT* indexMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&indexMap);
	//�S�C���f�b�N�X�ɑ΂���
	for (UINT i = 0; i < size; i++)
	{
		indexMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	view.BufferLocation = buff->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = dataSize;
}

void IndexBuffer::Init(std::vector<unsigned int> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	UINT dataSize = static_cast<UINT>(sizeof(UINT) * list.size());

	//�C���f�b�N�X�o�b�t�@���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�C���f�b�N�X�o�b�t�@����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff)
	);

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	UINT* indexMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&indexMap);
	//�S�C���f�b�N�X�ɑ΂���
	for (UINT i = 0; i < list.size(); i++)
	{
		indexMap[i] = list[i];
	}
	buff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	view.BufferLocation = buff->GetGPUVirtualAddress();
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = dataSize;
}
