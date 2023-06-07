#pragma once
#include <array>
#include <bitset>
#include "Vector2.h"

namespace ColPrimitive2D {

	//����
	struct Line {
		Vector2 p; //������̈�_
		Vector2 v; //�����̕����x�N�g��
	};

	//���C(������)
	struct Ray {
		Vector2 p; //���C�̎n�_
		Vector2 v; //���C�̕����x�N�g��
	};

	//����
	struct Segment {
		Vector2 p; //�����̎n�_
		Vector2 v; //�����̏I�_�֌������x�N�g��

		Segment() {}
		Segment(Vector2 a, Vector2 b) : p(a), v(b - a) {}

		bool operator==(const Segment& a) {
			return this->p == a.p && this->v == a.v;
		}
	};

	//��`
	struct Rect {
		Vector2 p; //��`�̒��S
		float w; //��(���a�I)
		float h; //����(���a�I)

		Rect() : w(0), h(0) {}
		Rect(Vector2 p, float w, float h) : p(p), w(w), h(h) {}

		//4���_�̔z��֕ϊ�
		//����A�E��A�����A�E��
		std::array<Vector2, 4> ToPointArray();

		//4�ӂ����ꂼ������Ƃ��ĕ\���z��֕ϊ�
		std::array<Segment, 4> ToSideArray();
	};

	struct ColResult {
		bool hit = false;
		bool hasHitPos = false;
		Vector2 hitPos; //�Փ˓_
		int hitSide = -1; //����������(0���珇�ɍ��E�㉺)
		std::bitset<4> hitSideBit = 0; //����������(4�r�b�g��A���ʂ��獶�E�㉺)
	};

	/// <summary>
	/// ��`�Ƌ�`�̓����蔻��
	/// </summary>
	/// <param name="a">��`A</param>
	/// <param name="b">��`B</param>
	/// <returns>�������Ă��邩�ǂ���</returns>
	bool IsHit(Rect a, Rect b);

	/// <summary>
	/// �����Ƌ�`�̓����蔻��
	/// </summary>
	/// <param name="seg">����</param>
	/// <param name="rect">��`</param>
	/// <returns>�������Ă��邩�ǂ���</returns>
	bool IsHit(Segment seg, Rect rect);

	/// <summary>
	/// �����Ƌ�`�̏Փˏ��𓾂�
	/// </summary>
	/// <param name="seg">����</param>
	/// <param name="rect">��`</param>
	/// <returns>�ڂ����Փˏ��</returns>
	ColResult CheckHit(Segment seg, Rect rect);
}