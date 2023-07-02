#pragma once
#include "Vector3.h"
#include "Color.h"
class PointLight
{
public:
	struct LightBuffer {
		bool active; // 4byte
		Vector3 pos; // 12byte(total 16)
		Vector3 color; // 12byte(total 28)
		float pad; // 4byte(total 32)
		Vector3 atten; //12byte(total 44)
		float pad2; //4byte(total 48)
	};

	Vector3 mPos;
	Color mColor;
	Vector3 mAtten; //ãóó£å∏êäåWêî
	bool mIsActive = false;

	void TransferBuffer(LightBuffer& buffer) {
		buffer.active = mIsActive;
		buffer.pos = mPos;
		buffer.color = {mColor.r, mColor.g, mColor.b};
		buffer.atten = mAtten;
	}
};

