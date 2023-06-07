#pragma once
#include <array>
#include <bitset>
#include "Vector2.h"

namespace ColPrimitive2D {

	//直線
	struct Line {
		Vector2 p; //直線上の一点
		Vector2 v; //直線の方向ベクトル
	};

	//レイ(半直線)
	struct Ray {
		Vector2 p; //レイの始点
		Vector2 v; //レイの方向ベクトル
	};

	//線分
	struct Segment {
		Vector2 p; //線分の始点
		Vector2 v; //線分の終点へ向かうベクトル

		Segment() {}
		Segment(Vector2 a, Vector2 b) : p(a), v(b - a) {}

		bool operator==(const Segment& a) {
			return this->p == a.p && this->v == a.v;
		}
	};

	//矩形
	struct Rect {
		Vector2 p; //矩形の中心
		float w; //幅(直径的)
		float h; //高さ(直径的)

		Rect() : w(0), h(0) {}
		Rect(Vector2 p, float w, float h) : p(p), w(w), h(h) {}

		//4頂点の配列へ変換
		//左上、右上、左下、右下
		std::array<Vector2, 4> ToPointArray();

		//4辺をそれぞれ線分として表す配列へ変換
		std::array<Segment, 4> ToSideArray();
	};

	struct ColResult {
		bool hit = false;
		bool hasHitPos = false;
		Vector2 hitPos; //衝突点
		int hitSide = -1; //当たった面(0から順に左右上下)
		std::bitset<4> hitSideBit = 0; //当たった面(4ビット列、下位から左右上下)
	};

	/// <summary>
	/// 矩形と矩形の当たり判定
	/// </summary>
	/// <param name="a">矩形A</param>
	/// <param name="b">矩形B</param>
	/// <returns>当たっているかどうか</returns>
	bool IsHit(Rect a, Rect b);

	/// <summary>
	/// 線分と矩形の当たり判定
	/// </summary>
	/// <param name="seg">線分</param>
	/// <param name="rect">矩形</param>
	/// <returns>当たっているかどうか</returns>
	bool IsHit(Segment seg, Rect rect);

	/// <summary>
	/// 線分と矩形の衝突情報を得る
	/// </summary>
	/// <param name="seg">線分</param>
	/// <param name="rect">矩形</param>
	/// <returns>詳しい衝突情報</returns>
	ColResult CheckHit(Segment seg, Rect rect);
}