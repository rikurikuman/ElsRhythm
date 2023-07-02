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
	float radius; //circle�Ŏg�p

	bool operator==(const DrawCustomData& l) const {
		return layer == l.layer
			&& fillFlag == l.fillFlag
			&& radius == l.radius;
	}
};

//unordered_map�p�Ƀn�b�V���֐�OBJ��ǉ����Ă�����
namespace std {
	template<>
	class hash<DrawCustomData> {
	public:
		size_t operator() (const DrawCustomData& o) const {
			return static_cast<size_t>((
				o.layer * 2.0f +
				static_cast<float>(o.fillFlag) * 3.0f +
				o.radius * 5.0f
			));
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
	std::vector<uint32_t> index;
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