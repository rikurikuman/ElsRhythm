#include "RhythmGameController.h"
#include "TimeManager.h"
#include <SimpleDrawer.h>
#include <RInput.h>
#include <RImGui.h>
#include <Camera.h>

float RhythmGameController::GetScroll(float miliSec)
{
	std::pair<Beat, float> nowScroll({ 0, 0, 1 }, 1.0f);

	for (std::pair<Beat, float> change : scrollChange) {
		if (miliSec < music.ConvertBeatToMiliSeconds(change.first)) {
			break;
		}
		
		nowScroll = change;
	}

	return nowScroll.second;
}

float RhythmGameController::GetPosition(float miliSec)
{
	float base = scrollBase * scrollSpeed;
	float pos = 0;

	std::pair<Beat, float> nowScroll({ 0, 0, 1 }, 1.0f);

	for (std::pair<Beat, float> change : scrollChange) {
		if (miliSec < music.ConvertBeatToMiliSeconds(change.first)) {
			break;
		}

		auto cache = cacheScrollPos.find(change.first);
		if (cache == cacheScrollPos.end()) {
			pos += (base * nowScroll.second) * (music.ConvertBeatToMiliSeconds(change.first) - music.ConvertBeatToMiliSeconds(nowScroll.first));
			cacheScrollPos[change.first] = pos;
		}
		else {
			pos = (*cache).second;
		}

		nowScroll = change;
	}

	pos += (base * nowScroll.second) * (miliSec - music.ConvertBeatToMiliSeconds(nowScroll.first));
	return pos;
}

void RhythmGameController::DrawMeasureLine()
{
	Beat beat = music.ConvertMiliSecondsToBeat(time);
	for (int32_t measure = beat.measure; ;) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds({ measure, 0, 1 })) - nowPosY);

		if (posY < -50 || posY > 770) {
			break;
		}

		if (abs(beat.measure - measure) > 1000) {
			Util::DebugLog("あぶない");
			break;
		}

		SimpleDrawer::DrawBox(
			0,
			static_cast<int32_t>(posY),
			RWindow::GetWidth(),
			static_cast<int32_t>(posY - 2),
			50, 0xffffff, true);
		measure--;
	}
	for (int32_t measure = beat.measure + 1; ;) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds({ measure, 0, 1 })) - nowPosY);

		if (posY < -50 || posY > 770) {
			break;
		}

		if (abs(beat.measure - measure) > 1000) {
			Util::DebugLog("あぶない");
			break;
		}

		SimpleDrawer::DrawBox(
			0,
			static_cast<int32_t>(posY),
			RWindow::GetWidth(),
			static_cast<int32_t>(posY - 2),
			50, 0xffffff, true);
		measure++;
	}
	for (std::pair<Beat, float> change : scrollChange) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds(change.first)) - nowPosY);

		if (posY < -50 || posY > 770) {
			continue;
		}

		SimpleDrawer::DrawBox(
			0,
			static_cast<int32_t>(posY),
			RWindow::GetWidth(),
			static_cast<int32_t>(posY - 2),
			50, 0xffffff, true);
	}
}

void RhythmGameController::Reset()
{
	playing = false;
	startedMusicStream = false;

	musicHandle = -1;
	music = MusicDesc();

	scrollChange.clear();
	cacheScrollPos.clear();

	notes.clear();
	remainNotes.clear();

	countJudgePerfect = 0;
	countJudgeHit = 0;
	countJudgeMiss = 0;

	time = -3000;
	offsetStream = 0;

	nowPosY = 0;
}

void RhythmGameController::Init()
{
	if (noteModelObjs.empty()) {
		Model::Load("./Resources/Model/Note", "note.obj", "Note");
		for (int32_t i = 0; i < 100; i++) {
			noteModelObjs.emplace_back("Note");
		}
	}

	remainNotes.clear();
	for (Note& note : notes) {
		remainNotes.push_back(note);
	}

	remainNotes.sort();
}

void RhythmGameController::Update()
{
	if (Util::debugBool) {
		if (RInput::GetKey(DIK_LSHIFT) || RInput::GetKey(DIK_RSHIFT)) {
			if (RInput::GetKeyDown(DIK_LEFT)) {
				scrollSpeed -= 1.0f;
			}

			if (RInput::GetKeyDown(DIK_RIGHT)) {
				scrollSpeed += 1.0f;
			}
		}
		else {
			if (RInput::GetKeyDown(DIK_LEFT)) {
				scrollSpeed -= 0.1f;
			}

			if (RInput::GetKeyDown(DIK_RIGHT)) {
				scrollSpeed += 0.1f;
			}
		}
	}

	{
		Beat beat = music.ConvertMiliSecondsToBeat(time);

		ImGui::SetNextWindowSize({ 400, 400 });
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 860, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("RhythmGameController", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			playing = false;
			//TODO:StopSoundMem(musicHandle);
			//TODO:SetSoundCurrentTime(0, musicHandle);
			startedMusicStream = false;
			chart.Load();
			Load();
			Init();
		}

		ImGui::Separator();

		ImGui::InputFloat("Time", &time);
		ImGui::Text(Util::StringFormat("BPM:%.2f", music.GetBPM(beat)).c_str());
		ImGui::Text(Util::StringFormat("Meter:%d/%d", music.GetMeter(beat).beatAmount, music.GetMeter(beat).beatLength).c_str());
		ImGui::Text(Util::StringFormat("Beat:%d:%d", beat.measure, beat.beat + 1).c_str());
		if (ImGui::Button("+1M##Beat")) {
			playing = false;
			//TODO:StopSoundMem(musicHandle);
			Beat beat = music.ConvertMiliSecondsToBeat(time);
			time += music.GetMeasureLength(beat.measure);
			if (startedMusicStream) {
				//TODO:SetSoundCurrentTime(time - offsetStream, musicHandle);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("+1B##Beat")) {
			playing = false;
			//TODO:StopSoundMem(musicHandle);
			Beat beat = music.ConvertMiliSecondsToBeat(time);
			time += music.GetMeasureLength(beat.measure) / beat.LPB;
			if (startedMusicStream) {
				//TODO:SetSoundCurrentTime(time - offsetStream, musicHandle);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("-1B##Beat")) {
			playing = false;
			//TODO:StopSoundMem(musicHandle);
			Beat beat = music.ConvertMiliSecondsToBeat(time);
			time -= music.GetMeasureLength(beat.measure) / beat.LPB;
			if (startedMusicStream) {
				//TODO:SetSoundCurrentTime(time - offsetStream, musicHandle);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("-1M##Beat")) {
			playing = false;
			//TODO:StopSoundMem(musicHandle);
			Beat beat = music.ConvertMiliSecondsToBeat(time);
			time -= music.GetMeasureLength(beat.measure);
			if (startedMusicStream) {
				//TODO:SetSoundCurrentTime(time - offsetStream, musicHandle);
			}
		}
		ImGui::Text(Util::StringFormat("Scroll:%.1f * %.2f", scrollSpeed, GetScroll(time)).c_str());
		ImGui::Spacing();
		ImGui::Text(Util::StringFormat("Perfect:%d", countJudgePerfect).c_str());
		ImGui::Text(Util::StringFormat("Hit:%d", countJudgeHit).c_str());
		ImGui::Text(Util::StringFormat("Hoge:%d", countJudgeMiss).c_str());
		ImGui::Text(Util::StringFormat("(RemainNotes:%d)", remainNotes.size()).c_str());

		ImGui::Separator();

		ImGui::Checkbox("AutoPlay", &autoplay);
		if (ImGui::Button("Play")) playing = true;
		ImGui::SameLine();
		if (ImGui::Button("Stop")) playing = false;

		ImGui::End();
	}

	if (playing) {
		//TODO:
		/*if (musicHandle != -1 && CheckSoundMem(musicHandle)) {
			time = offsetStream + GetSoundCurrentTime(musicHandle);
		}
		else {
			time += deltaMiliTime;
			if (musicHandle != -1 && !startedMusicStream && time >= offsetStream) {
				startedMusicStream = true;
				SetSoundCurrentTime(time - offsetStream, musicHandle);
				PlaySoundMem(musicHandle, DX_PLAYTYPE_BACK, false);
			}
		}*/
		time += TimeManager::deltaMiliTime;
	}
	else {
		if (musicHandle != -1) {
			//TODO:StopSoundMem(musicHandle);
		}
	}

	nowPosY = GetPosition(time);

	SimpleDrawer::DrawLine(0, posJudgeLine, RWindow::GetWidth(), posJudgeLine, 45, 0xff00ff, 5);

	for (int32_t lane = 0; lane < 5; lane++) {
		int32_t posX = laneCenter + (-laneWidth * 2 + laneWidth * lane);

		SimpleDrawer::DrawLine(posX, 0, posX, 720, 40, 0xffffff, 3);
	}

	int32_t laneKey[4] = { DIK_F, DIK_G, DIK_H, DIK_J };
	for (int32_t lane = 0; lane < 4; lane++) {
		if (RInput::GetKeyDown(laneKey[lane])) {
			int32_t posX = laneCenter + (-laneWidth * 2 + laneWidth * lane);
			SimpleDrawer::DrawBox(
				posX,
				0,
				posX + laneWidth,
				posJudgeLine,
				40, Color(0.8f, 0.8f, 0.8f, 0.15f), true, 3);
		}
	}

	SimpleDrawer::DrawBox(0, 0, laneCenter - laneWidth * 2, 720, 70, 0x111111, true);
	SimpleDrawer::DrawBox(laneCenter + laneWidth * 2, 0, 1280, 720, 70, 0x111111, true);

	Beat beat = music.ConvertMiliSecondsToBeat(time);

	bool existNearby[4] = {};
	Note nearbyNotes[4] = {};

	int32_t noteObjIndex = 0;
	for (Note& note : remainNotes) {
		if (existNearby[note.lane]) {
			float diffA = abs(time - music.ConvertBeatToMiliSeconds(nearbyNotes[note.lane].beat));
			float diffB = abs(time - music.ConvertBeatToMiliSeconds(note.beat));
			if (diffB < diffA) {
				nearbyNotes[note.lane] = note;
			}
		}
		else {
			nearbyNotes[note.lane] = note;
			existNearby[note.lane] = true;
		}
	}

	std::list<Note> removeNotes;
	for (Note& note : remainNotes) {
		int32_t posX = laneCenter + (-laneWidth * 2 + laneWidth * note.lane);
		if (!note.posMemed) {
			note.posMem = GetPosition(music.ConvertBeatToMiliSeconds(note.beat));
			note.posMemed = true;
		}
		float posY = posJudgeLine - (note.posMem - nowPosY);

		if (playing) {
			if (note.judged && note.judgeDiff <= judgePerfect && (music.ConvertBeatToMiliSeconds(note.beat) - time) <= 0) {
				//TODO:PlaySE("JudgePerfect");
				//TODO:PlayParticle(Particle::Spawn, posX + laneWidth / 2, posJudgeLine);
				//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine, "Perfect!", 0xffff00);
				countJudgePerfect++;
				removeNotes.push_back(note);
				continue;
			}
			else if (!note.judged) {
				float diffA = time - music.ConvertBeatToMiliSeconds(note.beat);
				float diffB = abs(diffA);

				if (existNearby[note.lane] && nearbyNotes[note.lane] == note) {
					if (RInput::GetKeyDown(laneKey[note.lane]) || (autoplay && diffA >= -judgeUltimatePerfect)) {
						if (diffB <= judgeHit * 2) {
							note.judged = true;
							note.judgeDiff = diffB;

							if (diffB <= judgePerfect) {
								//何もしない
								if (Util::debugBool) {
									//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine + 20, StringFormat("%+.1f", diffA) + "ms", diffA > 0 ? 0xff0000 : 0x00ffff);
								}
							}
							else if (diffB <= judgeHit) {
								//TODO:PlaySE("JudgeHit");
								//TODO:PlayParticle(Particle::Despawn, posX + laneWidth / 2, posY);
								//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine, "Hit", 0x00ff00);
								//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine + 20, StringFormat("%+.1f", diffA) + "ms", diffA > 0 ? 0xff0000 : 0x00ffff);
								countJudgeHit++;
								removeNotes.push_back(note);
								continue;
							}
							else {
								//TODO:PlaySE("JudgeMiss");
								//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine, "Hoge", 0xaaaaaa);
								countJudgeMiss++;
								removeNotes.push_back(note);
								continue;
							}
						}
					}
				}

				if (diffA >= judgeHit * 2) {
					//TODO:PlaySE("JudgeMiss");
					//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine, "Hoge", 0xaaaaaa);
					countJudgeMiss++;
					removeNotes.push_back(note);
					continue;
				}
			}
		}

		if (posY < -50 || posY > 770) {
			continue;
		}

		SimpleDrawer::DrawBox(
			static_cast<int32_t>(posX),
			static_cast<int32_t>(posY),
			static_cast<int32_t>(posX + laneWidth),
			static_cast<int32_t>(posY - 20),
			55, 0x0055ff, true);
		SimpleDrawer::DrawBox(
			static_cast<int32_t>(posX + 3),
			static_cast<int32_t>(posY - 3),
			static_cast<int32_t>(posX + laneWidth - 3),
			static_cast<int32_t>(posY - 17),
			55, 0x00ffff, true);

		if (noteObjIndex == noteModelObjs.size()) {
			noteModelObjs.emplace_back("Note");
		}
		ModelObj& obj = noteModelObjs[noteObjIndex];
		obj.mTransform.position = { static_cast<float>(0), 0, -posY / 90.0f };
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.Draw();
		noteObjIndex++;
	}

	for (Note& note : removeNotes) {
		remainNotes.remove(note);
	}

	DrawMeasureLine();
}

void RhythmGameController::Load()
{
	if (!chart.audiopath.empty()) {
		//TODO:musicHandle = LoadSoundMem(chart.audiopath.c_str());
	}

	offsetStream = chart.audioOffset;

	music = chart.music;
	scrollChange = chart.scrollChange;
	cacheScrollPos.clear();

	notes = chart.notes;
}

void RhythmGameController::Save()
{
	chart.audioOffset = offsetStream;

	chart.music = music;
	chart.scrollChange = scrollChange;

	chart.notes = notes;
	chart.Save();
}
