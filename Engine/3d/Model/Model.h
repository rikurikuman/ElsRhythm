#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RConstBuffer.h"
#include "Material.h"

//assimp
#pragma warning(push)
#pragma warning(disable: 26451)
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#pragma warning(pop)

typedef std::string ModelHandle;

class ModelData
{
public:
	std::string name;
	VertexBuffer vertexBuff;
	IndexBuffer indexBuff;

	std::vector<VertexPNU> vertexs;
	std::vector<UINT> indices;
	Material material;

	void CalcSmoothedNormals();

	bool operator==(const ModelData& o) const;
	bool operator!=(const ModelData& o) const;
};

class Model
{
public:
	std::string name;
	std::string path;
	std::vector<std::shared_ptr<ModelData>> data;

	static ModelHandle Load(std::string filepath, std::string filename, ModelHandle handle = "", bool smooth = false);
	static ModelHandle Register(ModelHandle handle, Model model);

	static ModelHandle LoadWithAIL(std::string directoryPath, std::string filename, ModelHandle handle = "");
};

class ModelManager
{
public:
	ModelManager() {
		Init();
	}

	static ModelManager* GetInstance() {
		static ModelManager manager;
		return &manager;
	}

	static Model* Get(ModelHandle handle);

	std::map<ModelHandle, Model> modelMap;
	std::recursive_mutex mutex;

private:
	void Init();
};