#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "Vertex.h"

class VertexBuffer
{
public:
	VertexBuffer() {};

	//Vertex(Pos)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@�����
	VertexBuffer(VertexP* list, uint32_t size);
	//Vertex(Pos)��vector�Œ��_�o�b�t�@�����
	VertexBuffer(std::vector<VertexP> list);

	//Vertex(PosNormalUv)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@�����
	VertexBuffer(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)��vector�Œ��_�o�b�t�@�����
	VertexBuffer(std::vector<VertexPNU> list);

	//Vertex(Pos)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@�����
	void Init(VertexP* list, uint32_t size);
	//Vertex(Pos)��vector�Œ��_�o�b�t�@�����
	void Init(std::vector<VertexP> list);

	//Vertex(PosNormalUv)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@�����
	void Init(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)��vector�Œ��_�o�b�t�@�����
	void Init(std::vector<VertexPNU> list);

	//Vertex(PosNormalUv)�̔z��Ƃ��̑傫���Œ��_�o�b�t�@���X�V����
	void Update(VertexPNU* list, uint32_t size);

	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW mView{};
};

