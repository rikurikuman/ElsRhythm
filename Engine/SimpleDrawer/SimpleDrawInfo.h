#pragma once
#include "Vector2.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RConstBuffer.h"
#include "SRConstBuffer.h"
#include "Material.h"
#include "ViewProjection.h"
#include "Sprite.h"

struct DrawBuffers {
	SRVertexBuffer vert;
	SRIndexBuffer index;
	SRVertexBuffer instance;
};

struct DrawCustomData {
	float layer;
	bool fillFlag;
	float radius; //circleで使用

	bool operator==(const DrawCustomData& l) const {
		return this->layer == l.layer
			&& this->fillFlag == l.fillFlag
			&& this->radius == l.radius;
	}
};

//unordered_map用にハッシュ関数OBJを追加してあげる
namespace std {
	template<>
	class hash<DrawCustomData> {
	public:
		size_t operator() (const DrawCustomData& o) const {
			return static_cast<size_t>(
				o.layer * 2.0f +
				o.fillFlag * 3.0f +
				o.radius * 5.0f
			);
		}
	};
}

struct SimpleDrawInfo {};

struct DrawBoxInfo {
	Vector2 start;
	Vector2 end;
	Color color;
};

struct DrawLineInfo {
	Vector2 start;
	Vector2 end;
	float thickness;
	Color color;
};

struct DrawCircleInfo {
	Vector2 center;
	Color color;
};

struct DrawCircleVertIndex {
	std::vector<VertexP> vert;
	std::vector<UINT> index;
};

struct DrawLine3DInfo {
	Vector3 start;
	Vector3 end;
	float width;
	float height;
	Vector3 upVec;
	Color color;
};

class DrawStringInfo : public SimpleDrawInfo {
public:
	Sprite sprite;
};