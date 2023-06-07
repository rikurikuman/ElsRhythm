#pragma once
#include "Color.h"
#include "RConstBuffer.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class LightGroup
{
public:
	static LightGroup* nowLight;
	static constexpr int directionalLightNum = 8;
	static constexpr int pointLightNum = 8;
	static constexpr int spotLightNum = 8;

	struct LightGroupBuffer {
		Vector3 ambientColor;
		float pad;
		DirectionalLight::LightBuffer directionalLights[directionalLightNum];
		PointLight::LightBuffer pointLights[pointLightNum];
		SpotLight::LightBuffer spotLights[spotLightNum];
	};

	RConstBuffer<LightGroupBuffer> buffer;

	LightGroup() {
		SetDefault();
	}

	//�f�t�H���g�l�ɐݒ�
	void SetDefault();

	//�X�V����
	void Update();

	//�����̐F��ݒ肷��
	void SetAmbientColor(Vector3 color);

	//���݂̊����̐F�𓾂�
	Vector3 GetAmbientColor();

	//���s�����̃A�N�e�B�u��Ԃ�ݒ肷��
	void SetDirectionalLightActive(int index, bool active);

	//���s�����̃A�N�e�B�u��Ԃ𓾂�
	bool GetDirectionalLightActive(int index);

	//���s�����̕�����ݒ肷��
	void SetDirectionalLightVec(int index, Vector3 vec);

	//���s�����̕����𓾂�
	Vector3 GetDirectionalLightVec(int index);

	//���s�����̐F��ݒ肷��
	void SetDirectionalLightColor(int index, Color color);

	//���s�����̐F�𓾂�
	Color GetDirectionalLightColor(int index);

	//�_�����̃A�N�e�B�u��Ԃ�ݒ肷��
	void SetPointLightActive(int index, bool active);

	//�_�����̃A�N�e�B�u��Ԃ𓾂�
	bool GetPointLightActive(int index);

	//�_�����̍��W��ݒ肷��
	void SetPointLightPos(int index, Vector3 pos);

	//�_�����̍��W�𓾂�
	Vector3 GetPointLightPos(int index);

	//�_�����̐F��ݒ肷��
	void SetPointLightColor(int index, Color color);

	//�_�����̐F�𓾂�
	Color GetPointLightColor(int index);

	//�_�����̌����W����ݒ肷��
	void SetPointLightAtten(int index, Vector3 atten);

	//�_�����̌����W���𓾂�
	Vector3 GetPointLightAtten(int index);

	//�X�|�b�g���C�g�̃A�N�e�B�u��Ԃ�ݒ肷��
	void SetSpotLightActive(int index, bool active);

	//�X�|�b�g���C�g�̃A�N�e�B�u��Ԃ𓾂�
	bool GetSpotLightActive(int index);

	//�X�|�b�g���C�g�̍��W��ݒ肷��
	void SetSpotLightPos(int index, Vector3 pos);

	//�X�|�b�g���C�g�̍��W�𓾂�
	Vector3 GetSpotLightPos(int index);

	//�X�|�b�g���C�g�̌�����ݒ肷��
	void SetSpotLightDirection(int index, Vector3 dir);

	//�X�|�b�g���C�g�̍��W�𓾂�
	Vector3 GetSpotLightDirection(int index);

	//�X�|�b�g���C�g�̐F��ݒ肷��
	void SetSpotLightColor(int index, Color color);

	//�X�|�b�g���C�g�̐F�𓾂�
	Color GetSpotLightColor(int index);

	//�X�|�b�g���C�g�̌����W����ݒ肷��
	void SetSpotLightAtten(int index, Vector3 atten);

	//�X�|�b�g���C�g�̌����W���𓾂�
	Vector3 GetSpotLightAtten(int index);

	//�X�|�b�g���C�g�̌����p�x��ݒ肷��
	void SetSpotLightFactorAngle(int index, Vector2 factorAngle);

	//�X�|�b�g���C�g�̌����p�x�𓾂�
	Vector2 GetSpotLightFactorAngle(int index);

private:
	//�����̐F
	Vector3 ambientColor = { 1, 1, 1 };

	//���s�����z��
	DirectionalLight directionalLights[directionalLightNum];

	//�_�����z��
	PointLight pointLights[pointLightNum];

	//�X�|�b�g���C�g�z��
	SpotLight spotLights[spotLightNum];

	//�X�V�t���O
	bool change = false;
};