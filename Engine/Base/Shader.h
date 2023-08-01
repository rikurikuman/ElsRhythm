#pragma once
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>
#include <unordered_map>

class Shader
{
public:
	Microsoft::WRL::ComPtr<ID3DBlob> mShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> mErrorBlob;
	bool mSucceeded = false;

	Shader() {}
	Shader(std::string filename, std::string entrypoint, std::string target);

	static void Register(std::string id, Shader shader);
	static Shader GetOrCreate(std::string id, std::string filename, std::string entrypoint, std::string target);
	static Shader GetRegistered(std::string id);

private:
	class ShaderRegister {
	public:
		std::unordered_map<std::string, Shader> mShaderRegister;
		static ShaderRegister* GetInstance() {
			static ShaderRegister reg;
			return &reg;
		}
	};
};

