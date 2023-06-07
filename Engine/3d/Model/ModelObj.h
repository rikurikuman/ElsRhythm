#pragma once
#include "Obj3D.h"
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "RConstBuffer.h"
#include "SRConstBuffer.h"
#include <map>

class RenderOrder;

class ModelObj : public Obj3D
{
public:
	Model* model = nullptr;
	Material tuneMaterial = Material(true);

	std::unordered_map<std::string, SRConstBuffer<MaterialBuffer>> materialBuffMap;
	SRConstBuffer<TransformBuffer> transformBuff;
	SRConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	ModelObj() {};
	~ModelObj() {}
	ModelObj(Model* model) : model(model) {};
	ModelObj(ModelHandle handle) : model(ModelManager::Get(handle)) {};

	//�e�f�[�^�̃o�b�t�@�ւ̓]��
	void TransferBuffer(ViewProjection viewprojection) override;

	//�`��v����Renderer��
	void Draw();
};

