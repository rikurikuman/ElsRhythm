#pragma once
#include <d3d12.h>
#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include <Viewport.h>
#include <Rect.h>
#include <Vector3.h>
#include <SRBuffer.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>

enum class RootDataType {
	UNDEFINED,
	DATA_32BIT_CONSTANTS,
	DESCRIPTOR_TABLE,
	CBV,
	SRV,
	UAV,
	SRBUFFER_CBV,
	CAMERA,
	LIGHT,
};

struct RootData {
	RootDataType type = RootDataType::UNDEFINED;
	union {
		D3D12_GPU_DESCRIPTOR_HANDLE descriptor;
		D3D12_GPU_VIRTUAL_ADDRESS address;
		SRBufferPtr addressSRBuff;
	};

	RootData() 
		: type(RootDataType::UNDEFINED), address(0) {}
	RootData(D3D12_GPU_DESCRIPTOR_HANDLE& descriptor)
		: type(RootDataType::DESCRIPTOR_TABLE), descriptor(descriptor) {}
	RootData(RootDataType type)
		: type(type), address(0) {}
	RootData(RootDataType type, D3D12_GPU_VIRTUAL_ADDRESS address)
		: type(type), address(address) {}
	RootData(RootDataType type, SRBufferPtr& ptr)
		: type(type), addressSRBuff(ptr) {}
};

class RenderOrder
{
public:
	Vector3 anchorPoint;
	std::vector<std::string> renderTargets;
	std::vector<Viewport> viewports;
	std::vector<Rect> scissorRects;
	D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* pipelineState = nullptr;
	SRVertexBuffer vertBuff;
	SRIndexBuffer indexBuff;
	SRVertexBuffer instanceVertBuff;
	D3D12_VERTEX_BUFFER_VIEW* vertView = nullptr; //SRVertexBufferを使わない場合はこっちにビューを直で
	D3D12_INDEX_BUFFER_VIEW* indexView = nullptr; //SRIndexBufferを使わない場合はこっちにビューを直で
	D3D12_VERTEX_BUFFER_VIEW* instanceVertView = nullptr;
	UINT indexCount = 0;
	UINT instanceCount = 1;
	std::vector<RootData> rootData;
};

