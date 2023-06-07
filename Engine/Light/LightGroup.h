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

	//デフォルト値に設定
	void SetDefault();

	//更新処理
	void Update();

	//環境光の色を設定する
	void SetAmbientColor(Vector3 color);

	//現在の環境光の色を得る
	Vector3 GetAmbientColor();

	//平行光源のアクティブ状態を設定する
	void SetDirectionalLightActive(int index, bool active);

	//平行光源のアクティブ状態を得る
	bool GetDirectionalLightActive(int index);

	//平行光源の方向を設定する
	void SetDirectionalLightVec(int index, Vector3 vec);

	//平行光源の方向を得る
	Vector3 GetDirectionalLightVec(int index);

	//平行光源の色を設定する
	void SetDirectionalLightColor(int index, Color color);

	//平行光源の色を得る
	Color GetDirectionalLightColor(int index);

	//点光源のアクティブ状態を設定する
	void SetPointLightActive(int index, bool active);

	//点光源のアクティブ状態を得る
	bool GetPointLightActive(int index);

	//点光源の座標を設定する
	void SetPointLightPos(int index, Vector3 pos);

	//点光源の座標を得る
	Vector3 GetPointLightPos(int index);

	//点光源の色を設定する
	void SetPointLightColor(int index, Color color);

	//点光源の色を得る
	Color GetPointLightColor(int index);

	//点光源の減衰係数を設定する
	void SetPointLightAtten(int index, Vector3 atten);

	//点光源の減衰係数を得る
	Vector3 GetPointLightAtten(int index);

	//スポットライトのアクティブ状態を設定する
	void SetSpotLightActive(int index, bool active);

	//スポットライトのアクティブ状態を得る
	bool GetSpotLightActive(int index);

	//スポットライトの座標を設定する
	void SetSpotLightPos(int index, Vector3 pos);

	//スポットライトの座標を得る
	Vector3 GetSpotLightPos(int index);

	//スポットライトの向きを設定する
	void SetSpotLightDirection(int index, Vector3 dir);

	//スポットライトの座標を得る
	Vector3 GetSpotLightDirection(int index);

	//スポットライトの色を設定する
	void SetSpotLightColor(int index, Color color);

	//スポットライトの色を得る
	Color GetSpotLightColor(int index);

	//スポットライトの減衰係数を設定する
	void SetSpotLightAtten(int index, Vector3 atten);

	//スポットライトの減衰係数を得る
	Vector3 GetSpotLightAtten(int index);

	//スポットライトの減衰角度を設定する
	void SetSpotLightFactorAngle(int index, Vector2 factorAngle);

	//スポットライトの減衰角度を得る
	Vector2 GetSpotLightFactorAngle(int index);

private:
	//環境光の色
	Vector3 ambientColor = { 1, 1, 1 };

	//平行光源配列
	DirectionalLight directionalLights[directionalLightNum];

	//点光源配列
	PointLight pointLights[pointLightNum];

	//スポットライト配列
	SpotLight spotLights[spotLightNum];

	//更新フラグ
	bool change = false;
};