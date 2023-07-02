#pragma once
#include "Color.h"
#include "RConstBuffer.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class LightGroup
{
public:
	static LightGroup* sNowLight;
	static constexpr int32_t DIRECTIONAL_LIGHT_NUM = 8;
	static constexpr int32_t POINT_LIGHT_NUM = 8;
	static constexpr int32_t SPOT_LIGHT_NUM = 8;

	struct LightGroupBuffer {
		Vector3 ambientColor;
		float pad;
		DirectionalLight::LightBuffer directionalLights[DIRECTIONAL_LIGHT_NUM];
		PointLight::LightBuffer pointLights[POINT_LIGHT_NUM];
		SpotLight::LightBuffer spotLights[SPOT_LIGHT_NUM];
	};

	RConstBuffer<LightGroupBuffer> mBuffer;

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
	void SetDirectionalLightActive(int32_t index, bool active);

	//���s�����̃A�N�e�B�u��Ԃ𓾂�
	bool GetDirectionalLightActive(int32_t index);

	//���s�����̕�����ݒ肷��
	void SetDirectionalLightVec(int32_t index, Vector3 vec);

	//���s�����̕����𓾂�
	Vector3 GetDirectionalLightVec(int32_t index);

	//���s�����̐F��ݒ肷��
	void SetDirectionalLightColor(int32_t index, Color color);

	//���s�����̐F�𓾂�
	Color GetDirectionalLightColor(int32_t index);

	//�_�����̃A�N�e�B�u��Ԃ�ݒ肷��
	void SetPointLightActive(int32_t index, bool active);

	//�_�����̃A�N�e�B�u��Ԃ𓾂�
	bool GetPointLightActive(int32_t index);

	//�_�����̍��W��ݒ肷��
	void SetPointLightPos(int32_t index, Vector3 pos);

	//�_�����̍��W�𓾂�
	Vector3 GetPointLightPos(int32_t index);

	//�_�����̐F��ݒ肷��
	void SetPointLightColor(int32_t index, Color color);

	//�_�����̐F�𓾂�
	Color GetPointLightColor(int32_t index);

	//�_�����̌����W����ݒ肷��
	void SetPointLightAtten(int32_t index, Vector3 atten);

	//�_�����̌����W���𓾂�
	Vector3 GetPointLightAtten(int32_t index);

	//�X�|�b�g���C�g�̃A�N�e�B�u��Ԃ�ݒ肷��
	void SetSpotLightActive(int32_t index, bool active);

	//�X�|�b�g���C�g�̃A�N�e�B�u��Ԃ𓾂�
	bool GetSpotLightActive(int32_t index);

	//�X�|�b�g���C�g�̍��W��ݒ肷��
	void SetSpotLightPos(int32_t index, Vector3 pos);

	//�X�|�b�g���C�g�̍��W�𓾂�
	Vector3 GetSpotLightPos(int32_t index);

	//�X�|�b�g���C�g�̌�����ݒ肷��
	void SetSpotLightDirection(int32_t index, Vector3 dir);

	//�X�|�b�g���C�g�̍��W�𓾂�
	Vector3 GetSpotLightDirection(int32_t index);

	//�X�|�b�g���C�g�̐F��ݒ肷��
	void SetSpotLightColor(int32_t index, Color color);

	//�X�|�b�g���C�g�̐F�𓾂�
	Color GetSpotLightColor(int32_t index);

	//�X�|�b�g���C�g�̌����W����ݒ肷��
	void SetSpotLightAtten(int32_t index, Vector3 atten);

	//�X�|�b�g���C�g�̌����W���𓾂�
	Vector3 GetSpotLightAtten(int32_t index);

	//�X�|�b�g���C�g�̌����p�x��ݒ肷��
	void SetSpotLightFactorAngle(int32_t index, Vector2 factorAngle);

	//�X�|�b�g���C�g�̌����p�x�𓾂�
	Vector2 GetSpotLightFactorAngle(int32_t index);

private:
	//�����̐F
	Vector3 mAmbientColor = { 1, 1, 1 };

	//���s�����z��
	DirectionalLight mDirectionalLights[DIRECTIONAL_LIGHT_NUM];

	//�_�����z��
	PointLight mPointLights[POINT_LIGHT_NUM];

	//�X�|�b�g���C�g�z��
	SpotLight mSpotLights[SPOT_LIGHT_NUM];

	//�X�V�t���O
	bool mChangeFlag = false;
};