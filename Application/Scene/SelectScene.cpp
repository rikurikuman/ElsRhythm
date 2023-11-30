#include "SelectScene.h"
#include <RInput.h>
#include <RImGui.h>
#include <RWindow.h>
#include <SimpleDrawer.h>
#include <SceneManager.h>
#include <GameScene.h>
#include <SimpleSceneTransition.h>
#include <TimeManager.h>

int32_t StrToInt(const std::string& str, int base = 10) {
	const char* p = str.c_str();
	char* end;
	errno = 0;
	long x = std::strtol(p, &end, base);
	if (p == end) {
		return INT_MAX;
	}
	if (errno == ERANGE || x < INT_MIN || x > INT_MAX) {
		return INT_MAX;
	}
	return static_cast<int>(x);
}

SelectScene::SelectScene()
{
	RAudio::Load("Resources/SOund/CursorMove.wav", "CursorMove");
	RAudio::Load("Resources/SOund/Decide.wav", "Decide");

	TextureManager::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute("Resources/defArtwork.png")), "defArtwork");
	TextureManager::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute("Resources/cblevelback_e.png")), "cblevelback_easy");
	TextureManager::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute("Resources/cblevelback_n.png")), "cblevelback_normal");
	TextureManager::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute("Resources/cblevelback_h.png")), "cblevelback_hard");

	mBackHexa.SetTexture(TextureManager::Load("./Resources/ptHexagon.png", "BackHexa"));

	std::filesystem::path chartFolder = PathUtil::ConvertAbsolute(L"./Charts");

	{
		ChartInfoFile tutorial;
		tutorial.displayName = "チュートリアル";
		tutorial.composerName = "りくりくまん";
		tutorial.charterName = "りくりくまん";
		tutorial.displayBPM = "95";
		tutorial.artworkPath = Util::ConvertWStringToString(PathUtil::ConvertAbsolute("Resources/tutorialArt.png"));
		tutorial.difficulties["easy"].num = 1;
		tutorial.difficulties["easy"].displayLevel = "1";
		tutorial.difficulties["easy"].chartPath = Util::ConvertWStringToString(PathUtil::ConvertAbsolute("./tutorial.kasu", chartFolder).c_str());
		mData2.push_back(tutorial);
	}

	SearchChartInfo(chartFolder);

	for (auto& itr : mData2) {
		for (auto& diff : itr.difficulties) {
			ChartButton button;
			button.name = itr.displayName;
			if (!diff.second.displayName.empty()) button.name = diff.second.displayName;

			if (!itr.artworkPath.empty()) {
				button.artTexHandle = TextureManager::Load(
						Util::ConvertWStringToString(
							PathUtil::ConvertAbsolute(Util::ConvertStringToWString(itr.artworkPath),
								itr.path
						).c_str()), button.name);
			}
			else {
				button.artTexHandle = "defArtwork";
			}
			button.artwork.SetTexture(button.artTexHandle);

			button.info = itr;
			button.difficulty = diff.second.num;
			button.selectDifficulty = button.difficulty;
			button.pos = { 1500, 0 };

			button.back.SetTexture(TextureManager::Load("Resources/cbback.png", "cbback"));
			mData3.push_back(button);
		}
	}

	std::stable_sort(++mData3.begin(), mData3.end(), [](ChartButton const& lhs, ChartButton const& rhs) {
		std::string levelA;
		if (lhs.info.difficulties.find(ChartInfoFile::GetDifficultyName(lhs.difficulty)) != lhs.info.difficulties.end())
			levelA = lhs.info.difficulties.at(ChartInfoFile::GetDifficultyName(lhs.difficulty)).displayLevel;
		std::string levelB;
		if (rhs.info.difficulties.find(ChartInfoFile::GetDifficultyName(rhs.difficulty)) != rhs.info.difficulties.end())
			levelB = rhs.info.difficulties.at(ChartInfoFile::GetDifficultyName(rhs.difficulty)).displayLevel;

		int32_t levelIntA = INT_MAX;
		int32_t levelIntB = INT_MAX;
		levelIntA = StrToInt(levelA);
		levelIntB = StrToInt(levelB);

		if (levelIntA < levelIntB) return true;
		else if (levelIntA == levelIntB) {
			if (levelA < levelB) return true;
			else if (levelA == levelB) {
				std::string nameA = lhs.name;
				std::string nameB = rhs.name;
				for (char& c : nameA) {
					c = static_cast<char>(std::toupper(c));
				}
				for (char& c : nameB) {
					c = static_cast<char>(std::toupper(c));
				}
				if (nameA < nameB) return true;
			}
		}
		return false;
	});
}

void SelectScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void SelectScene::Update()
{
	if (GetActiveWindow() == RWindow::GetWindowHandle()
		&& !ImGui::GetIO().WantCaptureMouse) {
		if (RInput::GetMouseMove().z != 0) {
			if (RInput::GetMouseMove().z < 0) {
				mFlickVec.y += -50;
			}
			else {
				mFlickVec.y += 50;
			}
		}

		if (RInput::GetMouseClickDown(0)) {
			mDragStartPos = RInput::GetMousePos();
			mOldScroll = mScroll;
		}

		if (RInput::GetMouseClick(0) || RInput::GetMouseClickUp(0)) {
			Vector2 drag = RInput::GetMousePos() - mDragStartPos;

			if (drag.Length() > 5) {
				mScroll = mOldScroll + drag.y;

				if (RInput::GetMouseClickUp(0)) {
					mFlickVec = RInput::GetMouseMove();
				}
			}
			else {
				Vector2 mouse = RInput::GetMousePos();
				for (size_t i = 0; i < mData3.size(); i++) {
					ChartButton& button = mData3[i];
					if (mouse.x >= button.pos.x - 250 && mouse.x <= button.pos.x + 250
						&& mouse.y >= button.pos.y - sButtonHeight / 2.0f && mouse.y <= button.pos.y + sButtonHeight / 2.0f) {
						if (RInput::GetMouseClickUp(0)) {
							if (mSelectIndex == i) {
								if (!SceneManager::IsSceneChanging()) {
									RAudio::Stop(mPreviewAudioHandle);

									std::filesystem::path chartPath = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(button.info.difficulties[ChartInfoFile::GetDifficultyName(button.selectDifficulty)].chartPath), button.info.path);
									GameScene::sChartPath = chartPath.c_str();
									GameScene::sLevel = button.selectDifficulty;
									SceneManager::Change<GameScene, SimpleSceneTransition>();
									RAudio::Play("Decide");
								}
							}
							else {
								button.selectDifficulty = button.difficulty;
								mSelectArtwork.SetTexture(button.artTexHandle);

								RAudio::Stop(mPreviewAudioHandle);
								if (!button.info.difficulties[ChartInfoFile::GetDifficultyName(button.selectDifficulty)].previewAudioPath.empty()) {
									mPreviewAudioHandle = RAudio::Load(
										Util::ConvertWStringToString(PathUtil::ConvertAbsolute(button.info.difficulties[ChartInfoFile::GetDifficultyName(button.selectDifficulty)].previewAudioPath, button.info.path)),
										"SelectPreviewAudio");
								}
								else {
									mPreviewAudioHandle = RAudio::Load(
										Util::ConvertWStringToString(PathUtil::ConvertAbsolute(button.info.audioPath, button.info.path)),
										"SelectPreviewAudio");
								}
								if (mPreviewAudioHandle != "") {
									mPreviewAudioPosStart = button.info.previewAudioPosStart;
									mPreviewAudioPosEnd = button.info.previewAudioPosEnd;
									if (mPreviewAudioPosEnd - mPreviewAudioPosStart > 0) {
										RAudio::SetPlayRange(mPreviewAudioHandle, mPreviewAudioPosStart, mPreviewAudioPosEnd);
										RAudio::SetLoopRange(mPreviewAudioHandle, mPreviewAudioPosStart, mPreviewAudioPosEnd);
									}
									else {
										mPreviewAudioPosEnd = RAudio::GetLength(mPreviewAudioHandle);
										RAudio::SetLoopRange(mPreviewAudioHandle, mPreviewAudioPosStart, mPreviewAudioPosEnd);
									}
									RAudio::Play(mPreviewAudioHandle, 0, 1, true);
								}

								mSelectArtwork.mTransform.scale.x = 400.0f / TextureManager::Get(button.artTexHandle).mResource->GetDesc().Width;
								mSelectArtwork.mTransform.scale.y = 400.0f / TextureManager::Get(button.artTexHandle).mResource->GetDesc().Height;
								mTime = 0;

								mSelectIndex = i;
								RAudio::Play("CursorMove");
							}
						}
					}
				}
			}
		}
	}

	if (mFlickVec.LengthSq() != 0) {
		mFlickVec *= 0.9f;
		mScroll += mFlickVec.y;
	}

	if (!RInput::GetMouseClick(0)) {
		float topPos = RWindow::GetHeight() / 2.0f + mScroll - sButtonHeight;
		float bottomPos = RWindow::GetHeight() / 2.0f + mScroll + sInterval * (mData3.size() - 1) + sButtonHeight;

		if (topPos > sBorderSpace) {
			float diff = sBorderSpace - topPos;

			mScroll += diff / 10.0f;
			mFlickVec = { 0, 0 };
		}
		if (bottomPos < RWindow::GetHeight() - sBorderSpace) {
			float diff = RWindow::GetHeight() - sBorderSpace - bottomPos;

			mScroll += diff / 10.0f;
			mFlickVec = { 0, 0 };
		}
	}

	for (size_t i = 0; i < mData3.size(); i++) {
		ChartButton& info = mData3[i];

		info.pos.y = RWindow::GetHeight() / 2.0f + mScroll + sInterval * i;

		if (mSelectIndex == i) {
			float diff = 1030 - info.pos.x;
			info.pos.x += diff / 5.0f;
		}
		else {
			float diff = 1100 - info.pos.x;
			info.pos.x += diff / 5.0f;
		}
	}

	if (RAudio::IsPlaying(mPreviewAudioHandle)) {
		float length = mPreviewAudioPosEnd - mPreviewAudioPosStart;
		if (length > 0) {
			float t = min(1, (RAudio::GetCurrentPosition(mPreviewAudioHandle) - mPreviewAudioPosStart) / 0.5f);
			if (t >= 0) {
				RAudio::SetVolume(mPreviewAudioHandle, t);
			}

			float t2 = min(1, (RAudio::GetCurrentPosition(mPreviewAudioHandle) - mPreviewAudioPosStart - length + 1.0f) / 1.0f);
			if (t2 >= 0) {
				RAudio::SetVolume(mPreviewAudioHandle, 1 - t2);
			}
		}
	}
}

void SelectScene::Draw()
{
	SimpleDrawer::DrawString(0, 5, 0, "クリックで曲を選択", 0xffffff, "やさしさゴシックボールドV2", 32, {0, 0});

	SimpleDrawer::DrawBox(0, 0, RWindow::GetWidth(), RWindow::GetHeight(), -100, 0x001f1f, true);

	mBackHexa.mTransform.position = { 200, 650, -1 };
	mBackHexa.mTransform.rotation.z += Util::AngleToRadian(90) * TimeManager::deltaTime;
	mBackHexa.mTransform.UpdateMatrix();
	mBackHexa.mMaterial.mColor = { 0, 0.6f, 0.6f, 1 };
	mBackHexa.TransferBuffer();
	mBackHexa.Draw();

	for (ChartButton& info : mData3) {
		if (info.pos.y + sButtonHeight / 2.0f < 0
			|| info.pos.y - sButtonHeight / 2.0f > RWindow::GetHeight()) continue;

		info.artwork.mTransform.position = info.pos;
		info.artwork.mTransform.position.x -= 107;
		info.artwork.mTransform.scale.x = 150.0f / TextureManager::Get(info.artTexHandle).mResource->GetDesc().Width;
		info.artwork.mTransform.scale.y = 150.0f / TextureManager::Get(info.artTexHandle).mResource->GetDesc().Height;
		float f = 150.0f / TextureManager::Get(info.artTexHandle).mResource->GetDesc().Height;
		info.artwork.SetTexRect(0, static_cast<int32_t>(45 / f), static_cast<int32_t>(TextureManager::Get(info.artTexHandle).mResource->GetDesc().Width), static_cast<int32_t>(60 / f));
		info.artwork.mTransform.UpdateMatrix();
		info.artwork.TransferBuffer();
		info.artwork.Draw();
		
		info.back.mTransform.position = info.pos;
		info.back.mTransform.UpdateMatrix();
		info.back.TransferBuffer();
		info.back.Draw();

		if (info.selectDifficulty == 1) {
			info.levelback.SetTexture("cblevelback_easy");
		}
		else if (info.selectDifficulty == 2) {
			info.levelback.SetTexture("cblevelback_normal");
		}
		else if (info.selectDifficulty == 3) {
			info.levelback.SetTexture("cblevelback_hard");
		}
		info.levelback.mTransform.position = info.pos;
		info.levelback.mTransform.UpdateMatrix();
		info.levelback.TransferBuffer();
		info.levelback.Draw();

		SimpleDrawer::DrawString(info.pos.x - 205, info.pos.y, 0, info.info.difficulties[ChartInfoFile::GetDifficultyName(info.selectDifficulty)].displayLevel, 0x000000, "やさしさゴシックボールドV2", 32, {0.5f, 0.5f});
		SimpleDrawer::DrawString(info.pos.x - 50, info.pos.y, 10, info.name, 0xfff2aa, "", 20, { 0.0f, 0.5f });
	}

	if (IsChartSelected()) {
		ChartButton& button = mData3[mSelectIndex];

		std::string displayName = button.info.difficulties[ChartInfoFile::GetDifficultyName(button.selectDifficulty)].displayName;
		if (displayName.empty()) displayName = button.info.displayName;

		std::string composerName = button.info.difficulties[ChartInfoFile::GetDifficultyName(button.selectDifficulty)].composerName;
		if (composerName.empty()) composerName = button.info.composerName;

		std::string displayBPM = button.info.difficulties[ChartInfoFile::GetDifficultyName(button.selectDifficulty)].displayBPM;
		if (displayBPM.empty()) displayBPM = button.info.displayBPM;
		
		SimpleDrawer::DrawString(0, 100, 0, displayName, 0xffffff, "やさしさゴシックボールドV2", 64);
		SimpleDrawer::DrawString(0, 170, 0, composerName, 0xffffff, "やさしさゴシックボールドV2", 32);
		SimpleDrawer::DrawString(0, 202, 0, "BPM: " + displayBPM, 0xffffff, "やさしさゴシックボールドV2", 32);

		mSelectArtwork.mTransform.position = { 450, 400, 0 };
		mTime += TimeManager::deltaTime;
		mSelectArtwork.mTransform.rotation.z = Util::AngleToRadian(-5 + sinf(mTime) * 5);
		mSelectArtwork.mTransform.UpdateMatrix();
		mSelectArtwork.TransferBuffer();
		mSelectArtwork.Draw();
	}
}

void SelectScene::Finalize()
{
	RAudio::Stop(mPreviewAudioHandle);
}

void SelectScene::SearchChartInfo(std::filesystem::path path)
{
	if (std::filesystem::is_directory(path)) {
		std::filesystem::directory_iterator itr(path);
		for (auto file : itr) {
			if (file.is_regular_file()) {
				if (file.path().extension() == ".kasumaster") {
					ChartInfoFile f(file.path().c_str());
					if (f.Load()) {
						mData2.push_back(f);
					}
				}
			}
			else if (file.is_directory()) {
				SearchChartInfo(file.path());
			}
		}
	}
}

bool SelectScene::IsChartSelected()
{
	return mSelectIndex != ULONGLONG_MAX;
}
