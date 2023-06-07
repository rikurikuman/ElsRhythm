#pragma once
class Rect {
public:
	long left;
	long right;
	long top;
	long bottom;

	bool operator==(const Rect a) const {
		return left == a.left && right == a.right
			&& top == a.top && bottom == a.bottom;
	}

	bool operator!=(const Rect& a) const {
		return !operator==(a);
	}
};