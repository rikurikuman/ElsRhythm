#include "RDirectX.h"
#include "RWindow.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "RootSignature.h"
#include "Vertex.h"
#include "SimpleDrawer.h"
#include "RenderTarget.h"

using namespace std;
using namespace DirectX;

RDirectX* RDirectX::GetInstance() {
	static RDirectX instance;
	return &instance;
}

ID3D12Device* RDirectX::GetDevice()
{
	return GetInstance()->device.Get();
}

ID3D12GraphicsCommandList* RDirectX::GetCommandList()
{
	return GetInstance()->cmdList.Get();
}

ID3D12DescriptorHeap* RDirectX::GetSRVHeap()
{
	return GetInstance()->srvHeap.Get();
}

RootSignature RDirectX::GetDefRootSignature()
{
	return GetInstance()->rootSignature;
}

GraphicsPipeline RDirectX::GetDefPipeline()
{
	return GetInstance()->pipelineState;
}

void RDirectX::PreDraw()
{
	RenderTarget::SetToBackBuffer();

	//�v���~�e�B�u�`��ݒ�
	RDirectX::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//SRV�q�[�v�̐ݒ�R�}���h
	ID3D12DescriptorHeap* _heap = TextureManager::GetInstance()->GetSRVHeap();
	RDirectX::GetCommandList()->SetDescriptorHeaps(1, &_heap);

	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().ptr.Get());
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().ptr.Get());
}

void RDirectX::PostDraw()
{
	RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());

	RDirectX::RunDraw();

	RDirectX::OpenResorceBarrier(RDirectX::GetCurrentBackBufferResource());
}

size_t RDirectX::GetBackBufferSize()
{
	return GetInstance()->backBuffers.size();
}

void RDirectX::Init() {
	GetInstance()->InitInternal();
}

void RDirectX::InitInternal() {
	HRESULT result;

	//DXGI�t�@�N�g���[����
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	vector<ComPtr<IDXGIAdapter4>> adapters;
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//�p�t�H�[�}���X���������ɑS�ẴA�_�v�^�[��񋓂���
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		adapters.push_back(tmpAdapter);
	}

	//�Ó��ȃA�_�v�^�̑I��
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;

		//�A�_�v�^�[�̏����擾����
		adapters[i]->GetDesc3(&adapterDesc);

		//�\�t�g�E�F�A�f�o�C�X�����O���āA
		//�ŏ��Ɍ��������A�_�v�^�i�p�t�H�[�}���X���ł������j���̗p����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			tmpAdapter = adapters[i];
			break;
		}
	}

	//�Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&device));
		if (result == S_OK) {
			featureLevel = levels[i];
			break;
		}
	}


	//�R�}���h�A���P�[�^�𐶐�
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));
	assert(SUCCEEDED(result));


	//�R�}���h���X�g�𐶐�
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList));
	assert(SUCCEEDED(result));


	//�R�}���h�L���[
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//����
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue));
	assert(SUCCEEDED(result));


	//�X���b�v�`�F�[��
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = RWindow::GetWidth();
	swapChainDesc.Height = RWindow::GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //�F���̏���
	swapChainDesc.SampleDesc.Count = 1; //�}���`�T���v�����Ȃ�
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; //�o�b�N�o�b�t�@�p
	swapChainDesc.BufferCount = 2; //�o�b�t�@����2��
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //�t���b�v��͔j��
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//����
	ComPtr<IDXGISwapChain1> _swapChain1;
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(), RWindow::GetWindowHandle(), &swapChainDesc, nullptr, nullptr,
		&_swapChain1);
	_swapChain1.As(&swapChain);
	assert(SUCCEEDED(result));


	//�f�X�N���v�^�q�[�v(RTV)
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //�����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; //���\�̓��
	//����
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	assert(SUCCEEDED(result));


	//�o�b�N�o�b�t�@
	backBuffers.resize(swapChainDesc.BufferCount);

	//�X���b�v�`�F�[���̑S�Ẵo�b�t�@�ɑ΂��ď���
	for (size_t i = 0; i < backBuffers.size(); i++) {
		//�X���b�v�`�F�[������o�b�t�@���擾
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		//�f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//�����\���ŃA�h���X�����炷
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//�����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//�V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//�����_�[�^�[�Q�b�g�r���[�̐���
		device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}


	//�f�X�N���v�^�q�[�v(SRV)
	const size_t kMaxSRVCount = 1; //�V�F�[�_�[���\�[�X�r���[�̍ő��
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //�V�F�[�_�[���猩����
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//����
	srvHeap = nullptr;
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));


	//�[�x�o�b�t�@
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = RWindow::GetWidth();
	depthResourceDesc.Height = RWindow::GetHeight();
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //�f�v�X�X�e���V��

	//�[�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�l�̃N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	//�o�b�t�@����
	result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff)
	);
	assert(SUCCEEDED(result));

	//�q�[�v����
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; //�f�v�X�X�e���V��
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	assert(SUCCEEDED(result));

	//�r���[����
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);


	//�t�F���X�̐���
	UINT64 fenceVal = 0;
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//���[�g�V�O�l�`��
	// �f�X�N���v�^�����W�̐ݒ�
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�̐ݒ�
	RootParamaters rootParams(5);
	//�e�N�X�`�����W�X�^0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //�f�X�N���v�^�e�[�u��
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@0��(Material)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[1].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParams[1].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@1��(Transform)
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[2].Descriptor.ShaderRegister = 1; //�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@2��(ViewProjection)
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[3].Descriptor.ShaderRegister = 2; //�萔�o�b�t�@�ԍ�
	rootParams[3].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����
	//�萔�o�b�t�@3��(Light)
	rootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //�萔�o�b�t�@�r���[
	rootParams[4].Descriptor.ShaderRegister = 3; //�萔�o�b�t�@�ԍ�
	rootParams[4].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�V�F�[�_���猩����

	// �e�N�X�`���T���v���[�̐ݒ�
	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //���J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //�c�J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //���s�J��Ԃ��i�^�C�����O�j
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //���j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f; //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //�s�N�Z���V�F�[�_�[���炾��������

	// ���[�g�V�O�l�`���̐ݒ�
	rootSignature.desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignature.desc.RootParamaters = rootParams;
	rootSignature.desc.StaticSamplers = StaticSamplerDescs{samplerDesc};

	rootSignature.Create();

	//Basic�V�F�[�_�B�̓ǂݍ��݂ƃR���p�C��
	basicVS = Shader("Shader/BasicVS.hlsl", "main", "vs_5_0");
	if (!basicVS.succeeded) {
		string error;
		error.resize(basicVS.errorBlob->GetBufferSize());

		copy_n((char*)basicVS.errorBlob->GetBufferPointer(),
			basicVS.errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	basicPS = Shader("Shader/BasicPS.hlsl", "main", "ps_5_0");
	if (!basicPS.succeeded) {
		string error;
		error.resize(basicPS.errorBlob->GetBufferSize());

		copy_n((char*)basicPS.errorBlob->GetBufferPointer(),
			basicPS.errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	Shader::Register("basicVS", basicVS);
	Shader::Register("basicPS", basicPS);

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	InputLayout inputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	// �V�F�[�_�[�̐ݒ�
	pipelineState.desc.VS = basicVS;
	pipelineState.desc.PS = basicPS;

	// �T���v���}�X�N�̐ݒ�
	pipelineState.desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //�W��

	// ���X�^���C�U�̐ݒ�
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //�w�ʃJ�����O
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = true;
	pipelineState.desc.BlendState.AlphaToCoverageEnable = true;

	// �u�����h�X�e�[�g(����������)
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineState.desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //RGBA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	// ���_���C�A�E�g�̐ݒ�
	pipelineState.desc.InputLayout = inputLayout;

	// �}�`�̌`��ݒ�
	pipelineState.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// �f�v�X�X�e���V���X�e�[�g�ݒ�
	pipelineState.desc.DepthStencilState.DepthEnable = true;
	pipelineState.desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineState.desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineState.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���̑��̐ݒ�
	pipelineState.desc.NumRenderTargets = 1;
	pipelineState.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	pipelineState.desc.SampleDesc.Count = 1; //1�s�N�Z���ɂ�1��T���v�����O

	//�Z�b�g
	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();
}

UINT RDirectX::GetCurrentBackBufferIndex() {
	return GetInstance()->swapChain->GetCurrentBackBufferIndex();
}

ID3D12Resource* RDirectX::GetCurrentBackBufferResource() {
	return GetInstance()->backBuffers[GetCurrentBackBufferIndex()].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RDirectX::GetCurrentBackBufferHandle() {
	RDirectX* instance = GetInstance();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = instance->rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += static_cast<size_t>(GetCurrentBackBufferIndex()) * instance->device->GetDescriptorHandleIncrementSize(instance->rtvHeap->GetDesc().Type);

	return rtvHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE RDirectX::GetBackBufferDSVHandle()
{
	return GetInstance()->dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void RDirectX::OpenResorceBarrier(ID3D12Resource* resource) {
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = resource;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //Before:�\������
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //After:�`���
	RDirectX::GetCommandList()->ResourceBarrier(1, &barrierDesc);
}

void RDirectX::CloseResourceBarrier(ID3D12Resource* resource) {
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = resource;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; //Before:�`�悩��
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; //After:�\����
	RDirectX::GetCommandList()->ResourceBarrier(1, &barrierDesc);
}

void RDirectX::ClearBackBuffer(Color color)
{
	RDirectX* instance = GetInstance();

	FLOAT clearColor[] = { color.r, color.g, color.b, color.r };
	instance->cmdList->ClearRenderTargetView(GetCurrentBackBufferHandle(), clearColor, 0, nullptr);
	instance->cmdList->ClearDepthStencilView(
		instance->dsvHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
}

void RDirectX::RunDraw() {
	HRESULT result;

	RDirectX* instance = GetInstance();

	//���߂̃N���[�Y
	result = instance->cmdList->Close();
	assert(SUCCEEDED(result));
	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { instance->cmdList.Get() };
	instance->cmdQueue->ExecuteCommandLists(1, cmdLists);

	//�t���b�v
	result = instance->swapChain->Present(1, 0);
	assert(SUCCEEDED(result));

	instance->cmdQueue->Signal(instance->fence.Get(), ++instance->fenceVal);
	if (instance->fence->GetCompletedValue() != instance->fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		if (event != NULL) {
			instance->fence->SetEventOnCompletion(instance->fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		else {
			assert(event != NULL);
		}
	}

	//�L���[���N���A
	result = instance->cmdAllocator->Reset();
	assert(SUCCEEDED(result));
	// �ĂуR�}���h���X�g�𒙂߂鏀��
	result = instance->cmdList->Reset(instance->cmdAllocator.Get(), nullptr);
	assert(SUCCEEDED(result));

	TextureManager::EndFrameProcess();
	SimpleDrawer::ClearData();
}