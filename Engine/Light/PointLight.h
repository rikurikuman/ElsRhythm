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

	Vector3 pos;
	Color color;
	Vector3 atten; //ãóó£å∏êäåWêî
	bool active = false;

	void TransferBuffer(LightBuffer& buffer) {
		buffer.active = active;
		buffer.pos = pos;
		buffer.color = {color.r, color.g, color.b};
		buffer.atten = atten;
	}
};

