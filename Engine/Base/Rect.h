#pragma once
struct RRect {
	long left;
	long right;
	long top;
	long bottom;

	bool operator==(const RRect& a) const {
		return left == a.left && right == a.right
			&& top == a.top && bottom == a.bottom;
	}

	bool operator!=(const RRect& a) const {
		return !operator==(a);
	}

	RRect() = default;
	RRect(long left, long right, long top, long bottom)
		: left(left), right(right), top(top), bottom(bottom) {};
};
