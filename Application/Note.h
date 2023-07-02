#pragma once
#include "BeatUtil.h"

class Note
{
public:
	Beat beat = {0, 0, 1};
	int32_t lane = 0;
	bool judged = false;
	float judgeDiff = 0;

	bool posMemed = false;
	float posMem = 0;

	Note() {}
	Note(Beat beat, int32_t lane) : beat(beat), lane(lane) {}

	bool operator==(const Note& note) const {
		return beat == note.beat && lane == note.lane && judged == note.judged && judgeDiff == note.judgeDiff;
	}

	bool operator<(const Note& note) const {
		return beat < note.beat;
	}
};