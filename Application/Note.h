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

	//�����Beat���������Ƀ\�[�g����Ă�����̂Ƃ��܂��I�I�I
	std::vector<ControlPoint> controlPoints;

	int32_t lane = 0;
	bool judgeFlag = false;
	bool judgeFlagB = false; //2�ڂ��~�������Ƃ�����̂ŕʘg�i�ق�Ƃ͂₾�j
	float judgeTime = 0;
	float judgeTimeB = 0; //2�ڂ��~�������Ƃ�����̂ŕʘg�i�ق�Ƃ͂₾�j
	float judgeDiff = 0;
	float HoldTime = 0; //�z�[���h����p�ŕʘg

	bool posCalced = false;
	float posZMem = 0;

	Note() {}

	bool operator==(const Note& note) const {
		return mainBeat == note.mainBeat
			&& subBeat == note.subBeat
			&& mainPos == note.mainPos
			&& subPos == note.subPos
			&& lane == note.lane
			&& judgeFlag == note.judgeFlag
			&& judgeFlagB == note.judgeFlagB
			&& judgeDiff == note.judgeDiff
			&& HoldTime == note.HoldTime;
	}

	bool operator<(const Note& note) const {
		return mainBeat < note.mainBeat;
	}
};