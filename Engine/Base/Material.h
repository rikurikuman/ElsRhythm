#pragma once
#include "Color.h"
#include "Vector3.h"
#include <string>
#include "Texture.h"

struct MaterialBuffer {
	Color color;
	Vector3 ambient;
	float pad;
	Vector3 diffuse;
	float pad2;
	Vector3 specular;
};

class Material
{
public:
	std::string name; //���Ȃ܂��h�b�g�R��
	TextureHandle texture; //�e�N�X�`��
	Color color = {1, 1, 1, 1}; //�F(RGBA)
	Vector3 ambient = {1, 1, 1}; //����
	Vector3 diffuse = {0, 0, 0}; //�g�U���ˌ�
	Vector3 specular = {0, 0, 0}; //���ʔ��ˌ�

	Material() {}
	Material(bool allOne) {
		if (allOne) {
			diffuse = { 1, 1, 1 };
			specular = { 1, 1, 1 };
		}
	}
	bool operator==(const Material& o) const;
	bool operator!=(const Material& o) const;

	/// <summary>
	/// �萔�o�b�t�@�֓]��
	/// </summary>
	/// <param name="target">�Ώۂ̃o�b�t�@�ւ̃|�C���^</param>
	void Transfer(MaterialBuffer* target);
};