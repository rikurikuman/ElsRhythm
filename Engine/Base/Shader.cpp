#include "Shader.h"
#include "Util.h"
#include "RDirectX.h"

Shader::Shader(std::string filename, std::string entrypoint, std::string target)
{
	HRESULT result = D3DCompileFromFile(
		std::wstring(filename.begin(), filename.end()).c_str(), //ファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), //エントリ名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用
		0,
		&mShaderBlob, &mErrorBlob);
	mSucceeded = SUCCEEDED(result);
}

void Shader::Register(std::string id, Shader shader)
{
	ShaderRegister::GetInstance()->mShaderRegister[id] = shader;
}

Shader Shader::GetOrCreate(std::string id, std::string filename, std::string entrypoint, std::string target)
{
	Shader res = GetRegistered(id);
	if (!res.mSucceeded) {
		res = Shader(filename, entrypoint, target);
		Register(id, res);
	}
	return res;
}

Shader Shader::GetRegistered(std::string id)
{
	std::unordered_map<std::string, Shader>& map = ShaderRegister::GetInstance()->mShaderRegister;
	if (map.find(id) == map.end()) {
		return Shader();
	}
	return map[id];
}
