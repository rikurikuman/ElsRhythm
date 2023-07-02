#pragma once
#include "BeatUtil.h"
#include "Note.h"
#include "ChartFile.h"
#include <RWindow.h>
#include <ModelObj.h>

class RhythmGameController
{
public:
	ChartFile chart;

	bool playing = false;
	bool startedMusicStream = false;
	bool autoplay = false;

	int32_t musicHandle = -1;
	MusicDesc music;

	float scrollSpeed = 9;
	std::map<Beat, float> scrollChange;
	std::map<Beat, float> cacheScrollPos;

	std::list<Note> notes;
	std::list<Note> remainNotes;

	std::vector<ModelObj> noteModelObjs;

	int32_t countJudgePerfect = 0;
	int32_t countJudgeHit = 0;
	int32_t countJudgeMiss = 0;

	float time = -3000;
	float offsetStream = 0;

	float nowPosY = 0;

	RhythmGameController() {}

	float GetScroll(float miliSec);
	float GetPosition(float miliSec);

	void DrawMeasureLine();
	void Reset();
	void Init();
	void Update();

	void Load();
	void Save();

	static constexpr int32_t posJudgeLine = 620;
	static constexpr int32_t laneCenter = 1280 / 2;
	static constexpr int32_t laneWidth = 100;
	static constexpr float scrollBase = 0.1f;

	static constexpr float judgeUltimatePerfect = 25;
	static constexpr float judgePerfect = 50;
	static constexpr float judgeHit = 100;
};

