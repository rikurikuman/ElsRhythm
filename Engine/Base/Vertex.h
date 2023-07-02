#pragma once
#include <d3d12.h>
#include "Vector2.h"
#include "Vector3.h"
#include <vector>

struct VertexP
{
	Vector3 pos;

	VertexP(Vector3 pos = { 0, 0, 0 })
		: pos(pos) {}
};

struct VertexPU
{
	Vector3 pos;
	Vector2 uv;

	VertexPU(Vector3 pos = { 0, 0, 0 }, Vector2 uv = {0, 0})
		: pos(pos), uv(uv) {}
};

class VertexPNU
{
public:
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;

	VertexPNU(Vector3 pos = { 0, 0, 0 }, Vector3 normal = { 0, 0, 1 }, Vector2 uv = { 0, 0 })
		: pos(pos), normal(normal), uv(uv) {}

	bool operator==(const VertexPNU& a) const;

	/// <summary>
	/// �@���x�N�g�����v�Z
	/// </summary>
	/// <param name="list">�v�Z���钸�_�Q</param>
	/// <param name="indexlist">�C���f�b�N�X���X�g�i�O�p�`�A�S�Ď��v���ł��邱�Ɓj</param>
	/// <param name="indexcount">�C���f�b�N�X�̐�</param>
	static void CalcNormalVec(VertexPNU list[], const uint32_t indexlist[], const uint32_t indexcount);

	static void CalcNormalVec(std::vector<VertexPNU> list, std::vector<uint32_t> indexlist);
};

