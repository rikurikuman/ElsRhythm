#pragma once
#include "Vector3.h"
#include "RConstBuffer.h"

class DirectionalLight
{
public:
	struct LightBuffer
	{
		bool active; // 4byte
		Vector3 lightVec; // 12byte(total 16)
		Vector3 color; // 12byte(total 28)
		float pad; // 4byte(total 32)
	};

	Vector3 lightVec;
	Color color;
	bool active = false;

	DirectionalLight() : lightVec(Vector3(0, -1, 0)), color(Color(1, 1, 1, 1)) {}
	DirectionalLight(Vector3 lightVec, Color color) : lightVec(lightVec), color(color) {}

	void TransferBuffer(LightBuffer& buffer) {
		buffer.active = active;
		buffer.lightVec = lightVec;
		buffer.color = Vector3(color.r, color.g, color.b);
	}
};

