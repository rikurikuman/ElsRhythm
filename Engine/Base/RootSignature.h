#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <unordered_map>

typedef D3D12_DESCRIPTOR_RANGE DescriptorRange;
typedef D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc;
typedef std::vector<DescriptorRange> DescriptorRanges;
typedef std::vector<StaticSamplerDesc> StaticSamplerDescs;

struct RootParamater {
    D3D12_ROOT_PARAMETER_TYPE ParameterType;
    DescriptorRanges DescriptorTable;
    D3D12_ROOT_CONSTANTS Constants = {};
    D3D12_ROOT_DESCRIPTOR Descriptor = {};
    D3D12_SHADER_VISIBILITY ShaderVisibility = {};
};

typedef std::vector<RootParamater> RootParamaters;

struct RootSignatureDesc {
    RootParamaters RootParamaters;
    StaticSamplerDescs StaticSamplers;
    D3D12_ROOT_SIGNATURE_FLAGS Flags;
};

class RootSignature
{
public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mPtr = nullptr;
    RootSignatureDesc mDesc{};

	void Create();

    static RootSignature& GetOrCreate(std::string id, RootSignatureDesc desc);

private:
    static std::unordered_map<std::string, RootSignature> sRootSignatureMap;
};

