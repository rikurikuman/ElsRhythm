#include "RhythmGameController.h"
#include "TimeManager.h"
#include <SimpleDrawer.h>
#include <RInput.h>
#include <RImGui.h>
#include <Camera.h>
#include <ParticleExplode.h>

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
	int32_t lineObjIndex = 0;
	Beat beat = music.ConvertMiliSecondsToBeat(time);
	for (int32_t measure = beat.measure; ;) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds({ measure, 0, 1 })) - nowPosY);

		if (posY < -80 || posY > 50) {
			break;
		}

		if (abs(beat.measure - measure) > 1000) {
			Util::DebugLog("あぶない");
			break;
		}

		if (lineObjIndex == lineModelObjs.size()) {
			lineModelObjs.emplace_back("Cube");
		}
		ModelObj& obj = lineModelObjs[lineObjIndex];
		obj.mTransform.position = { 0, -0.035f, -posY };
		obj.mTransform.scale = { 16.0f, 0.1f, 0.1f };
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.Draw();
		lineObjIndex++;
		measure--;
	}
	for (int32_t measure = beat.measure + 1; ;) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds({ measure, 0, 1 })) - nowPosY);

		if (posY < -80 || posY > 50) {
			break;
		}

		if (abs(beat.measure - measure) > 1000) {
			Util::DebugLog("あぶない");
			break;
		}

		if (lineObjIndex == lineModelObjs.size()) {
			lineModelObjs.emplace_back("Cube");
		}
		ModelObj& obj = lineModelObjs[lineObjIndex];
		obj.mTransform.position = { 0, -0.035f, -posY };
		obj.mTransform.scale = { 16.0f, 0.1f, 0.1f };
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.Draw();
		lineObjIndex++;
		measure++;
	}
	for (std::pair<Beat, float> change : scrollChange) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds(change.first)) - nowPosY);

		if (posY < -80 || posY > 50) {
			continue;
		}

		if (lineObjIndex == lineModelObjs.size()) {
			lineModelObjs.emplace_back("Cube");
		}
		ModelObj& obj = lineModelObjs[lineObjIndex];
		obj.mTransform.position = { 0, -0.035f, -posY };
		obj.mTransform.scale = { 16.0f, 0.1f, 0.1f };
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.Draw();
		lineObjIndex++;
	}
}

void RhythmGameController::Reset()
{
	playing = false;
	startedMusicStream = false;

	audioHandle = "";
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

	if (arcNoteObjs.empty()) {
		for (int32_t i = 0; i < 100; i++) {
			arcNoteObjs.emplace_back();
		}
	}

	if (lineModelObjs.empty()) {
		Model::Load("./Resources/Model/", "cube.obj", "Cube");
		for (int32_t i = 0; i < 100; i++) {
			lineModelObjs.emplace_back("Cube");
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
			RAudio::Stop(audioHandle);
			RAudio::SetPlayRange(audioHandle, 0, 0);
			startedMusicStream = false;
			chart.Load();
			Load();
			Init();
		}

		ImGui::Separator();

		ImGui::DragFloat("Time", &time);
		ImGui::Text(Util::StringFormat("BPM:%.2f", music.GetBPM(beat)).c_str());
		ImGui::Text(Util::StringFormat("Meter:%d/%d", music.GetMeter(beat).beatAmount, music.GetMeter(beat).beatLength).c_str());
		ImGui::Text(Util::StringFormat("Beat:%d:%d(%d)", beat.measure, beat.beat + 1, beat.LPB).c_str());
		if (ImGui::Button("+1M##Beat")) {
			playing = false;
			RAudio::Stop(audioHandle);
			Beat beat = music.ConvertMiliSecondsToBeat(time);
			time += music.GetMeasureLength(beat.measure);
			if (startedMusicStream) {
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("+1B##Beat")) {
			playing = false;
			RAudio::Stop(audioHandle);
			Beat beat = music.ConvertMiliSecondsToBeat(time);
			time += music.GetMeasureLength(beat.measure) / beat.LPB;
			if (startedMusicStream) {
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("-1B##Beat")) {
			playing = false;
			RAudio::Stop(audioHandle);
			Beat beat = music.ConvertMiliSecondsToBeat(time);
			time -= music.GetMeasureLength(beat.measure) / beat.LPB;
			if (startedMusicStream) {
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("-1M##Beat")) {
			playing = false;
			RAudio::Stop(audioHandle);
			Beat beat = music.ConvertMiliSecondsToBeat(time);
			time -= music.GetMeasureLength(beat.measure);
			if (startedMusicStream) {
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
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
		if (!audioHandle.empty() && RAudio::IsPlaying(audioHandle)) {
			time = offsetStream + RAudio::GetCurrentPosition(audioHandle) * 1000.0f;
		}
		else {
			time += TimeManager::deltaMiliTime;
			if (!audioHandle.empty() && !startedMusicStream && time >= offsetStream) {
				startedMusicStream = true;
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
				RAudio::Play(audioHandle);
			}
		}
	}
	else {
		if (!audioHandle.empty()) {
			RAudio::Stop(audioHandle);
			startedMusicStream = false;
		}
	}

	nowPosY = GetPosition(time);

	int32_t laneKey[4] = { DIK_F, DIK_G, DIK_H, DIK_J };

	Beat beat = music.ConvertMiliSecondsToBeat(time);

	bool existNearby[4] = {};
	Note nearbyNotes[4] = {};

	int32_t noteObjIndex = 0;
	int32_t arcObjIndex = 0;
	//一番近いノーツを見つけます
	for (Note& note : remainNotes) {
		if (note.type == Note::Types::Arc) {
			continue;
		}
		if (existNearby[note.lane]) {
			float diffA = abs(time - music.ConvertBeatToMiliSeconds(nearbyNotes[note.lane].mainBeat));
			float diffB = abs(time - music.ConvertBeatToMiliSeconds(note.mainBeat));
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
		float posX = laneCenter + (-laneWidth * 2 + laneWidth * note.lane + laneWidth / 2);
		if (!note.posCalced) {
			if (note.type == Note::Types::Tap || note.type == Note::Types::Hold) {
				note.mainPos.x = laneCenter + (-laneWidth * 2 + laneWidth * note.lane + laneWidth / 2);
				note.mainPos.y = 0;
				note.subPos.x = note.mainPos.x;
				note.subPos.y = note.mainPos.y;
			}

			note.mainPos.z = GetPosition(music.ConvertBeatToMiliSeconds(note.mainBeat));
			if (note.type == Note::Types::Hold) {
				note.subPos.z = GetPosition(music.ConvertBeatToMiliSeconds(note.subBeat));
			}

			if (note.type == Note::Types::Arc) {
				for (auto& cp : note.controlPoints) {
					cp.pos.z = GetPosition(music.ConvertBeatToMiliSeconds(cp.beat));
					cp.posCalced = true;
				}
				note.subPos.z = GetPosition(music.ConvertBeatToMiliSeconds(note.subBeat));
			}

			note.posCalced = true;
		}
		float posZ = posJudgeLine - (note.mainPos.z - nowPosY);

		if (playing) {
			if (note.type == Note::Types::Tap) {
				if (note.judgeFlag && note.judgeDiff <= judgePerfect && (music.ConvertBeatToMiliSeconds(note.mainBeat) - time) <= 0) {
					RAudio::Play("JudgePerfect");
					//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine, "Perfect!", 0xffff00);
					for (int32_t i = 0; i < 12; i++) {
						ParticleExplode::Spawn({ posX, 0, 0 }, 0xffe32e, (360.0f / 12) + (360.0f / 12 * i), 10, 15, 0.5f);
					}
					countJudgePerfect++;
					removeNotes.push_back(note);
					continue;
				}
				else if (!note.judgeFlag) {
					float diffA = time - music.ConvertBeatToMiliSeconds(note.mainBeat);
					float diffB = abs(diffA);

					if (existNearby[note.lane] && nearbyNotes[note.lane] == note) {
						if (RInput::GetKeyDown(laneKey[note.lane])
							|| (note.lane == 2 && RInput::GetMouseClickDown(0))
							|| (note.lane == 3 && RInput::GetMouseClickDown(1))
							|| (autoplay && diffA >= -judgeUltimatePerfect)) {
							if (diffB <= judgeHit * 2) {
								note.judgeFlag = true;
								note.judgeDiff = diffB;

								if (diffB <= judgePerfect) {
									//何もしない
									if (Util::debugBool) {
										//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine + 20, StringFormat("%+.1f", diffA) + "ms", diffA > 0 ? 0xff0000 : 0x00ffff);
									}
								}
								else if (diffB <= judgeHit) {
									RAudio::Play("JudgeHit");
									//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine, "Hit", 0x00ff00);
									//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine + 20, StringFormat("%+.1f", diffA) + "ms", diffA > 0 ? 0xff0000 : 0x00ffff);
									for (int32_t i = 0; i < 12; i++) {
										ParticleExplode::Spawn({ posX, 0, 0 }, 0x77ff5e, (360.0f / 12) + (360.0f / 12 * i), 5, 15, 0.5f);
									}
									if (diffA > 0) {
										for (int32_t i = 0; i < 3; i++) {
											ParticleExplode::Spawn({ posX, 0, 0 }, 0xff0000, (360.0f / 3) + (360.0f / 3 * i), 5, 15, 0.5f);
										}
									}
									else {
										for (int32_t i = 0; i < 3; i++) {
											ParticleExplode::Spawn({ posX, 0, 0 }, 0x0000ff, (360.0f / 3) + (360.0f / 3 * i), 5, 15, 0.5f);
										}
									}
									countJudgeHit++;
									removeNotes.push_back(note);
									continue;
								}
								else {
									RAudio::Play("JudgeMiss");
									//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine, "Hoge", 0xaaaaaa);
									countJudgeMiss++;
									removeNotes.push_back(note);
									continue;
								}
							}
						}
					}

					if (diffA >= judgeHit * 2) {
						RAudio::Play("JudgeMiss");
						//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine, "Hoge", 0xaaaaaa);
						countJudgeMiss++;
						removeNotes.push_back(note);
						continue;
					}
				}
			}
			else if (note.type == Note::Types::Arc) {
				//現在のアークの節を取得する
				Beat arcStartBeat = note.mainBeat;
				Beat arcEndBeat = note.subBeat;
				Vector3 arcStartPos, arcEndPos;
				arcStartPos = note.mainPos;
				arcEndPos = note.subPos;
				for (auto& cp : note.controlPoints) {
					if (music.ConvertBeatToMiliSeconds(arcStartBeat) < music.ConvertBeatToMiliSeconds(cp.beat)
						&& music.ConvertBeatToMiliSeconds(cp.beat) < time) {
						arcStartBeat = cp.beat;
						arcStartPos = cp.pos;
					}
				}
				for (auto& cp : note.controlPoints) {
					if (music.ConvertBeatToMiliSeconds(arcStartBeat) < music.ConvertBeatToMiliSeconds(cp.beat)
						&& music.ConvertBeatToMiliSeconds(cp.beat) < music.ConvertBeatToMiliSeconds(arcEndBeat)) {
						arcEndBeat = cp.beat;
						arcEndPos = cp.pos;
					}
				}

				if (false) {

				}
				else {
					float diffA = time - music.ConvertBeatToMiliSeconds(note.mainBeat);
					float diffB = abs(diffA);

					if (music.ConvertBeatToMiliSeconds(note.subBeat) < time) {
						removeNotes.push_back(note);
					}

					if (diffA > 0) {
						Vector3 diffArcDir = (arcEndPos - arcStartPos);
						bool checkMouseDir = (diffArcDir.x == 0
							|| (RInput::GetMouseMove().x != 0 && RInput::GetMouseMove().x * diffArcDir.x >= 0))
							&& (diffArcDir.y == 0
								|| (RInput::GetMouseMove().y != 0 && RInput::GetMouseMove().y * diffArcDir.y < 0));

						if (checkMouseDir
							|| (autoplay && diffA >= -judgeUltimatePerfect)) {
							note.judgeFlag = true;
							note.judgeDiff = diffB;
							note.judgeBeat = arcStartBeat;

							float timeA = music.ConvertBeatToMiliSeconds(arcStartBeat);
							float timeB = music.ConvertBeatToMiliSeconds(arcEndBeat);
							float ratio = Util::GetRatio(timeA, timeB, time);

							Vector3 nowArcPos = arcStartPos + (arcEndPos - arcStartPos) * ratio;

							for (int32_t i = 0; i < 12; i++) {
								ParticleExplode::Spawn({nowArcPos.x, nowArcPos.y, 0}, 0xffe32e, (360.0f / 12) + (360.0f / 12 * i), 10, 15, 0.5f);
							}
						}
						else {
							note.judgeFlag = false;
						}
					}
				}
			}
		}

		if (note.type == Note::Types::Tap) {
			if (posZ < -80 || posZ > 50) {
				continue;
			}

			if (noteObjIndex == noteModelObjs.size()) {
				noteModelObjs.emplace_back("Note");
			}
			ModelObj& obj = noteModelObjs[noteObjIndex];
			obj.mTransform.position = { posX, 0, -posZ };
			obj.mTransform.UpdateMatrix();
			obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
			obj.Draw();
			noteObjIndex++;
		}
		
		if (note.type == Note::Types::Arc) {
			Vector3 prevPos = note.mainPos;
			
			Vector3 startPos;
			Vector3 endPos;

			for (auto& cp : note.controlPoints) {
				startPos = prevPos;
				endPos = cp.pos;
				startPos.z = posJudgeLine + (prevPos.z - nowPosY);
				endPos.z = posJudgeLine + (cp.pos.z - nowPosY);

				if (cp.beat < note.judgeBeat) {
					prevPos = cp.pos;
					continue;
				}

				if (startPos.z > 80 || endPos.z < -50) {
					prevPos = cp.pos;
					continue;
				}

				if (startPos.z < -50) {
					float ratio = Util::GetRatio(startPos.z, endPos.z, -50);
					startPos = startPos + (endPos - startPos) * ratio;
				}
				else if (note.judgeFlag && startPos.z < posJudgeLine && endPos.z > posJudgeLine) {
					float ratio = Util::GetRatio(startPos.z, endPos.z, posJudgeLine);
					startPos = startPos + (endPos - startPos) * ratio;
				}

				if (endPos.z > 80) {
					float ratio = Util::GetRatio(startPos.z, endPos.z, 80);
					endPos = startPos + (endPos - startPos) * ratio;
				}

				if (arcObjIndex == arcNoteObjs.size()) {
					arcNoteObjs.emplace_back();
				}
				ArcNoteObj& obj = arcNoteObjs[arcObjIndex];
				obj.mStartPos = startPos;
				obj.mGoalPos = endPos;
				obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
				obj.Draw();
				arcObjIndex++;
				prevPos = cp.pos;
			}

			startPos = prevPos;
			endPos = note.subPos;
			startPos.z = posJudgeLine + (prevPos.z - nowPosY);
			endPos.z = posJudgeLine + (note.subPos.z - nowPosY);

			if (startPos.z > 80 || endPos.z < -50) {
				continue;
			}

			if (startPos.z < -50) {
				float ratio = Util::GetRatio(startPos.z, endPos.z, -50);
				startPos = startPos + (endPos - startPos) * ratio;
			}

			if (endPos.z > 80) {
				float ratio = Util::GetRatio(startPos.z, endPos.z, 80);
				endPos = startPos + (endPos - startPos) * ratio;
			}

			if (arcObjIndex == arcNoteObjs.size()) {
				arcNoteObjs.emplace_back();
			}
			ArcNoteObj& obj = arcNoteObjs[arcObjIndex];
			obj.mStartPos = startPos;
			obj.mGoalPos = endPos;
			obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
			obj.Draw();
			arcObjIndex++;
		}
	}

	for (Note& note : removeNotes) {
		remainNotes.remove(note);
	}

	DrawMeasureLine();

	SimpleDrawer::DrawString(60, 100, 0, Util::StringFormat("BPM:%.2f", music.GetBPM(beat)), {1, 1, 1, 1}, "", 80);
	SimpleDrawer::DrawString(60, 180, 0, Util::StringFormat("Perfect:%d", countJudgePerfect));
	SimpleDrawer::DrawString(60, 200, 0, Util::StringFormat("Hit:%d", countJudgeHit));
	SimpleDrawer::DrawString(60, 220, 0, Util::StringFormat("Hoge:%d", countJudgeMiss));
}

void RhythmGameController::Load()
{
	if (!chart.audiopath.empty()) {
		audioHandle = RAudio::Load(chart.audiopath.c_str());
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
