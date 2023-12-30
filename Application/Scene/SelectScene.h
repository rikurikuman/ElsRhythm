/*
* @file SelectScene.h
* @brief 楽曲選択シーン
*/

#pragma once
#include "IScene.h"
#include <vector>
#include <string>
#include <Camera.h>
#include <LightGroup.h>
#include <Vector2.h>
#include <ChartInfoFile.h>
#include <PathUtil.h>
#include <Sprite.h>
#include <RAudio.h>

class SelectScene : public IScene
{
public:
	SelectScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	Camera camera = Camera();
	LightGroup light;

	struct MusicInfo {
		std::string name;
		Vector2 pos;
	};
	std::vector<MusicInfo> mData;
	std::list<ChartInfoFile> mData2;

	struct ChartButton {
		std::string name;
		ChartInfoFile info;
		int32_t difficulty;
		int32_t selectDifficulty;
		Vector2 pos;

		TextureHandle artTexHandle;
		Sprite back;
		Sprite levelback;
		Sprite artwork;
	};

	std::vector<ChartButton> mData3;
	size_t mSelectIndex = ULONGLONG_MAX;

	AudioHandle mPreviewAudioHandle;
	float mPreviewAudioPosStart = 0;
	float mPreviewAudioPosEnd = 0;
	Sprite mSelectArtwork;
	float mTime = 0;

	Sprite mBackHexa;

	Vector2 mDragStartPos;
	Vector2 mFlickVec;
	float mOldScroll = 0;
	float mScroll = 0;
	static constexpr float sBorderSpace = 20;
	static constexpr float sButtonHeight = 60;
	static constexpr float sInterval = 100;

	void SearchChartInfo(std::filesystem::path path);

	bool IsChartSelected();
};

