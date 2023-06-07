#include "ModelObj.h"
#include "LightGroup.h"
#include <Renderer.h>

void ModelObj::TransferBuffer(ViewProjection viewprojection)
{
	int count = 0;
	for (std::shared_ptr<ModelData> data : model->data) {
		std::string name = data->material.name;
		if (name.empty()) {
			name = "NoNameMaterial_" + count;
			data->material.name = name;
			count++;
		}
		Material mat = data->material;
		mat.color.r *= tuneMaterial.color.r;
		mat.color.g *= tuneMaterial.color.g;
		mat.color.b *= tuneMaterial.color.b;
		mat.color.a *= tuneMaterial.color.a;
		mat.ambient *= tuneMaterial.ambient;
		mat.diffuse *= tuneMaterial.diffuse;
		mat.specular *= tuneMaterial.specular;
		mat.Transfer(materialBuffMap[name].Get());
	}

	transform.Transfer(transformBuff.Get());
	viewprojection.Transfer(viewProjectionBuff.Get());
	//viewProjectionBuff.constMap->matrix = viewprojection.matrix;
}

void ModelObj::Draw()
{
	for (std::shared_ptr<ModelData> data : model->data) {
		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->material.texture).gpuHandle },
			{ RootDataType::SRBUFFER_CBV, materialBuffMap[data->material.name].buff },
			{ RootDataType::SRBUFFER_CBV, transformBuff.buff },
			{ RootDataType::SRBUFFER_CBV, viewProjectionBuff.buff },
			{ RootDataType::LIGHT }
		};
		

		std::string stage = "Opaque";
		if (data->material.color.a < 1.0f || tuneMaterial.color.a < 1.0f) stage = "Transparent";
		Renderer::DrawCall(stage, &data->vertexBuff.view, &data->indexBuff.view, static_cast<UINT>(data->indices.size()), rootData);
	}
	
}
