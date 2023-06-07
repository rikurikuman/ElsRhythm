#include "Shader.h"
#include "Util.h"
#include "RDirectX.h"

Shader::Shader(std::string filename, std::string entrypoint, std::string target)
{
	HRESULT result = D3DCompileFromFile(
		std::wstring(filename.begin(), filename.end()).c_str(), //�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), //�G���g�����A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //�f�o�b�O�p
		0,
		&shaderBlob, &errorBlob);
	succeeded = SUCCEEDED(result);
}

void Shader::Register(std::string id, Shader shader)
{
	ShaderRegister::GetInstance()->shaderRegister[id] = shader;
}

Shader Shader::GetOrCreate(std::string id, std::string filename, std::string entrypoint, std::string target)
{
	Shader res = GetRegistered(id);
	if (!res.succeeded) {
		res = Shader(filename, entrypoint, target);
		Register(id, res);
	}
	return res;
}

Shader Shader::GetRegistered(std::string id)
{
	std::unordered_map<std::string, Shader>& map = ShaderRegister::GetInstance()->shaderRegister;
	if (map.find(id) == map.end()) {
		return Shader();
	}
	return map[id];
}
