#pragma once
#include "Color.h"
#include "Vector3.h"
#include <string>
#include "Texture.h"

struct MaterialBuffer {
	Color color;
	Vector3 ambient;
	float pad = 0;
	Vector3 diffuse;
	float pad2 = 0;
	Vector3 specular;
};

class Material
{
public:
	std::string mName; //���Ȃ܂��h�b�g�R��
	TextureHandle mTexture; //�e�N�X�`��
	Color mColor = {1, 1, 1, 1}; //�F(RGBA)
	Vector3 mAmbient = {1, 1, 1}; //����
	Vector3 mDiffuse = {0, 0, 0}; //�g�U���ˌ�
	Vector3 mSpecular = {0, 0, 0}; //���ʔ��ˌ�

	Material() {}
	Material(bool allOne) {
		if (allOne) {
			mDiffuse = { 1, 1, 1 };
			mSpecular = { 1, 1, 1 };
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