#pragma once
#include "BeatUtil.h"
#include <Vector3.h>

class Note
{
public:
	enum class Types : int32_t {
		Tap = 0,
		Hold = 1,
		Arc = 2,
		Unknown = 3
	};

	struct ControlPoint {
		Beat beat;
		Vector3 pos;
		bool posCalced = false;
	};

	Types type = Types::Unknown;
	Beat mainBeat = { 0, 0, 1 };
	Beat subBeat = { 0, 0, 1 };
	Vector3 mainPos = { 0, 0, 0 };
	Vector3 subPos = { 0, 0, 0 };
	std::vector<ControlPoint> controlPoints;

	int32_t lane = 0;
	bool judgeFlag = false;
	Beat judgeBeat = { 0, 0, 1 };
	float judgeDiff = 0;

	bool posCalced = false;
	float posZMem = 0;

	Note() {}

	bool operator==(const Note& note) const {
		return mainBeat == note.mainBeat && lane == note.lane && judgeFlag == note.judgeFlag && judgeDiff == note.judgeDiff;
	}

	bool operator<(const Note& note) const {
		return mainBeat < note.mainBeat;
	}
};