#pragma once
#include "Vector3.h"
#include "Color.h"
#include "Vector2.h"
#include <math.h>
class SpotLight
{
public:
	struct LightBuffer {
		bool active; // 4byte
		Vector3 pos; // 12byte(total 16)
		Vector3 direction; //12byte(total 28)
		float pad; // 4byte(total 32)
		Vector3 color; // 12byte(total 44)
		float pad2; // 4byte(total 48)
		Vector3 atten; //12byte(total 60)
		float pad3; //4byte(total 64)
		Vector2 factorAngleCos; //8byte(total 72)
		float pad4[2]; //8byte(total 80)
	};

	Vector3 pos;
	Vector3 direction;
	Color color;
	Vector3 atten; //ãóó£å∏êäåWêî
	Vector2 factorAngle; //å∏êääpìx
	bool active = false;

	void TransferBuffer(LightBuffer& buffer) {
		buffer.active = active;
		buffer.pos = pos;
		buffer.direction = direction;
		buffer.color = { color.r, color.g, color.b };
		buffer.atten = atten;
		buffer.factorAngleCos.x = cosf(factorAngle.x);
		buffer.factorAngleCos.y = cosf(factorAngle.y);
	}
};

