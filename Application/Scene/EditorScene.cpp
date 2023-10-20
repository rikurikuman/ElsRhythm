#include "EditorScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>
#include <ParticleObject.h>
#include <SceneManager.h>
#include <TitleScene.h>
#include <SimpleSceneTransition.h>
#include <ParticleSprite3D.h>
#include <ResultScene.h>
#include <BackGroundSelector.h>
#include <EObjTapNote.h>
#include <EObjArcNote.h>
#include <RAudio.h>
#include <filesystem>

EditorScene::EditorScene()
{
	mJudgeLineObj = ModelObj("Cube");
	mJudgeLineObj.mTransform.position = { 0, -0.04f, 0 };
	mJudgeLineObj.mTransform.scale = { 16, 0.1f, 0.2f };
	mJudgeLineObj.mTransform.UpdateMatrix();
	mJudgeLineObj.mTuneMaterial.mColor = { 0.3f, 0, 0.3f, 1.0f };

	mLaneObj = ModelObj("Cube");
	mLaneObj.mTransform.position = { 0, -0.1f, 0 };
	mLaneObj.mTransform.scale = { 16, 0.1f, 160.0f };
	mLaneObj.mTransform.UpdateMatrix();
	mLaneObj.mTuneMaterial.mColor = { 0.2f, 0.2f, 0.2f, 1.0f };

	for (int32_t i = 0; i < 3; i++) {
		mLineObjs[i] = ModelObj("Cube");
		mLineObjs[i].mTransform.position = { -4.0f + 4.0f * i, -0.045f, 0 };
		mLineObjs[i].mTransform.scale = { 0.2f, 0.1f, 160.0f };
		mLineObjs[i].mTransform.UpdateMatrix();
		mLineObjs[i].mTuneMaterial.mColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	camera.mViewProjection.mEye = { 0, 11.0f, -12.0f };
	camera.mAngle = { 90, -25 };
	camera.mFreeFlag = true;
	camera.mViewProjection.UpdateMatrix();

	TextureManager::Load("./Resources/ptCircle.png", "ParticleCircle");
	TextureManager::Load("./Resources/judgeText.png", "judgeText");

	RAudio::Load("Resources/Sound/Judge_Perfect.wav", "JudgePerfect");
	RAudio::Load("Resources/Sound/Judge_Hit.wav", "JudgeHit");
	RAudio::Load("Resources/Sound/Judge_Miss.wav", "JudgeMiss");
	RAudio::Load("Resources/Sound/VerticalSlash.wav", "VerticalSlash");

	chartFile.Load();
}

void EditorScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void EditorScene::Finalize()
{
}

void EditorScene::Update()
{
	ShowWindow();

	int32_t laneSize = sMaxLaneX - sMinLaneX;
	int32_t laneCenter = (sMinLaneX + sMaxLaneX) / 2;
	Vector2 mouse = RInput::GetMousePos();

	if (mState == EditorState::Chart) {
		if (GetActiveWindow() == RWindow::GetWindowHandle()
			&& !ImGui::GetIO().WantCaptureMouse
			&& sMinFieldX <= mouse.x && mouse.x <= sMaxFieldX
			&& 0 <= mouse.y && mouse.y <= 720) {

			if (RInput::GetMouseMove().z != 0)
				mTime += 5.0f * RInput::GetMouseMove().z;

			if (RInput::GetMouseClickDown(0)) {
				mDragStartPos = mouse;
			}

			if (mMode == EditMode::Pen) {
				bool checkSelect = false;
				int32_t selectCount = 0;
				std::vector<IEditorObject*> selectList;
				for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end(); itr++) {
					IEditorObject* obj = itr->get();
					if (obj->IsSelected()) {
						checkSelect = true;
						selectCount += obj->GetSelectCountInThisInstance();
						selectList.push_back(obj);
					}
				}

				bool checkOnObject = false;
				for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end(); itr++) {
					IEditorObject* obj = itr->get();
					if (obj->Collide(RInput::GetMousePos().x, RInput::GetMousePos().y)) {
						checkOnObject = true;
						if (RInput::GetMouseClickDown(0)) {
							if (!(RInput::GetKey(DIK_LCONTROL) || RInput::GetKey(DIK_RCONTROL) || selectCount >= 2)) {
								for (auto itr2 = mEditorObjects.begin(); itr2 != mEditorObjects.end(); itr2++) {
									IEditorObject* obj2 = itr2->get();
									obj2->UnSelect();
								}
							}
							obj->Select(mouse.x, mouse.y, !RInput::GetKey(DIK_LCONTROL) && !RInput::GetKey(DIK_RCONTROL) && selectCount >= 2);
							break;
						}
					}
				}
				if (!checkOnObject && RInput::GetMouseClickDown(0)) {
					for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end(); itr++) {
						IEditorObject* obj = itr->get();
						obj->UnSelect();
					}
				}

				selectCount = 0;
				selectList.clear();
				for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end(); itr++) {
					IEditorObject* obj = itr->get();
					if (obj->IsSelected()) {
						checkSelect = true;
						selectCount += obj->GetSelectCountInThisInstance();
						selectList.push_back(obj);
					}
				}

				if (RInput::GetMouseClick(0)) {
					Vector2 drag = (mouse - mDragStartPos);
					float dragDistance = abs(drag.Length());

					for (IEditorObject* obj : selectList) {
						if (RInput::GetMouseClickDown(0)) {
							obj->MoveStart();
						}
						if (dragDistance >= 5) {
							obj->Move(drag.x, drag.y);
						}
					}
				}

				//何も選択してないなら追加の処理
				if (!checkOnObject && !checkSelect) {
					if (mEditType == EditType::TapNote) {
						if (mouse.x >= sMinLaneX && mouse.x <= sMaxLaneX) {
							Beat cursorBeat = GetCursorBeat();

							int32_t lane = 0;
							for (int i = 0; i <= 3; i++) {
								int32_t x1 = sMinLaneX + laneSize / 4 * i;
								int32_t x2 = sMinLaneX + laneSize / 4 * (i + 1);

								if (mouse.x >= x1 && mouse.x <= x2) {
									lane = i;
									break;
								}
							}

							EObjTapNote::TempDraw(this, lane, cursorBeat);

							if (RInput::GetMouseClickDown(0)) {
								mEditorObjects.push_back(std::make_unique<EObjTapNote>(this));
								EObjTapNote* obj = static_cast<EObjTapNote*>(mEditorObjects.back().get());
								obj->mLane = lane;
								obj->mBeat = cursorBeat;
							}
						}
					}
					else {
						if (mEditType == EditType::ArcNote) {
							if (mouse.x >= sMinLaneX && mouse.x <= sMaxLaneX) {
								Beat cursorBeat = GetCursorBeat();

								float nearX = 1;
								float nearDistance = abs(EditorScene::sMinLaneX - mouse.x);
								for (float tx = -8; tx <= 8; tx += 0.25f) {
									float sx = laneCenter + (tx / 8.0f) * (laneSize / 2);
									float distance = abs(sx - mouse.x);
									if (distance <= nearDistance) {
										nearX = tx;
										nearDistance = distance;
									}
								}

								EObjArcNote::TempDraw(this, nearX, cursorBeat);

								if (RInput::GetMouseClickDown(0)) {
									mEditorObjects.push_back(std::make_unique<EObjArcNote>(this));
									EObjArcNote* obj = static_cast<EObjArcNote*>(mEditorObjects.back().get());
									obj->mStartPos = { nearX, 1.0f };
									obj->mEndPos = { nearX, 1.0f };
									obj->mStartBeat = cursorBeat;
									obj->mEndBeat = { cursorBeat.measure + 1, 0, 1 };
								}
							}
						}
					}
				}
			}
			else if (mMode == EditMode::Erase) {
				if (RInput::GetMouseClickDown(0)) {
					bool checkSelect = false;
					//選択してるもの優先処理
					for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end();) {
						IEditorObject* obj = itr->get();
						if (obj->IsSelected() && obj->Collide(RInput::GetMousePos().x, RInput::GetMousePos().y)) {
							checkSelect = true;
							break;
						}

						itr++;
					}

					if (checkSelect) {
						for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end();) {
							IEditorObject* obj = itr->get();
							if (obj->IsSelected()) {
								itr = mEditorObjects.erase(itr);
								continue;
							}

							itr++;
						}
					}
					else {
						for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end();) {
							IEditorObject* obj = itr->get();
							if (RInput::GetMouseClickDown(0)) {
								if (obj->Collide(RInput::GetMousePos().x, RInput::GetMousePos().y)) {
									itr = mEditorObjects.erase(itr);
									break;
								}
							}

							itr++;
						}

						for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end(); itr++) {
							IEditorObject* obj = itr->get();
							obj->UnSelect();
						}
					}
				}
			}
		}
	}
	

	mNowScrollPos = GetPosition(mTime);

	light.Update();
	//camera.Update();
}

void EditorScene::Draw()
{
	//譜面編集状態でのみ
	if (mState == EditorState::Chart) {
		//レーン出すよ
		SimpleDrawer::DrawLine(sMinLaneX, 0, sMinLaneX, 720, 0, 0xffffff, 2);
		SimpleDrawer::DrawLine(sMaxLaneX, 0, sMaxLaneX, 720, 0, 0xffffff, 2);
		int32_t laneSize = sMaxLaneX - sMinLaneX;

		for (int i = 1; i <= 3; i++) {
			int32_t x = sMinLaneX + laneSize / 4 * i;
			SimpleDrawer::DrawLine(x, 0, x, 720, 0, 0xffffff, 2);
		}

		SimpleDrawer::DrawLine(sMinArcHeightX, 0, sMinArcHeightX, 720, 0, 0xffffff, 2);
		SimpleDrawer::DrawLine(sMaxArcHeightX, 0, sMaxArcHeightX, 720, 0, 0xffffff, 2);

		SimpleDrawer::DrawLine(sMinLaneX, sBaseLineY, sMaxLaneX, sBaseLineY, 1, 0xff00ff, 2);
		DrawMeasureLines();

		//DrawNotes();

		for (std::unique_ptr<IEditorObject>& obj : mEditorObjects) {
			obj->Draw();
		}
	}

	/*judgeLine.TransferBuffer(Camera::sNowCamera->mViewProjection);
	judgeLine.Draw();
	lane.TransferBuffer(Camera::sNowCamera->mViewProjection);
	lane.Draw();
	for (int32_t i = 0; i < 3; i++) {
		lines[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
		lines[i].Draw();
	}*/
}

float EditorScene::GetScreenPosition(Beat beat)
{
	return sBaseLineY - (GetPosition(chartFile.music.ConvertBeatToMiliSeconds(beat)) - mNowScrollPos);
}

Beat EditorScene::GetVirtualCursorBeat(float y)
{
	Beat cursorBeat = { chartFile.music.ConvertMiliSecondsToBeat(mTime).measure, 0, mSetLPB };
	float cursorDistance = abs(y - GetScreenPosition(cursorBeat));

	int32_t startMeasure = cursorBeat.measure;
	Beat checkBeat = cursorBeat;
	while (true) {
		checkBeat.beat++;
		if (checkBeat.beat >= mSetLPB) {
			checkBeat.beat = 0;
			checkBeat.measure++;
		}

		if (abs(checkBeat.measure - startMeasure) >= 100) {
			break;
		}

		float pos = GetScreenPosition(checkBeat);

		float distance = abs(y - pos);
		if (distance <= cursorDistance) {
			cursorBeat = checkBeat;
			cursorDistance = distance;
		}
		else {
			break;
		}
	}

	while (true) {
		checkBeat.beat--;
		if (checkBeat.beat < 0) {
			checkBeat.beat = mSetLPB;
			checkBeat.measure--;
		}

		if (abs(checkBeat.measure - startMeasure) >= 100) {
			break;
		}

		float pos = GetScreenPosition(checkBeat);

		float distance = abs(y - pos);
		if (distance <= cursorDistance) {
			cursorBeat = checkBeat;
			cursorDistance = distance;
		}
		else {
			break;
		}
	}
	return cursorBeat;
}


void EditorScene::ShowWindow()
{
	ShowMainWindow();
	ShowToolWindow();
}

void EditorScene::ShowMainWindow()
{
	ImGui::SetNextWindowSize({ 400, 380 }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	ImGui::Begin("Editor", NULL, window_flags);

	if (mState == EditorState::Chart) {
		ImGui::Text("楽曲ファイル");
		ImGui::InputText("##楽曲ファイル", &chartFile.audiopath);
		ImGui::SameLine();
		if (ImGui::Button("...##楽曲ファイル選択")) {
			wchar_t filePath[MAX_PATH] = { 0 };

			OPENFILENAME ofn = { 0 };
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = RWindow::GetWindowHandle();
			ofn.lpstrFilter =
				L"Waveファイル (.wav)\0*.wav\0"
				L"すべてのファイル (*.*)\0*.*\0";
			ofn.nFilterIndex = 0;
			ofn.lpstrFile = filePath;
			ofn.nMaxFile = MAX_PATH;

			if (GetOpenFileName(&ofn)) {
				std::wstring wStr = filePath;
				chartFile.audiopath = Util::ConvertWStringToString(wStr);
			}
		}
		ImGui::SameLine();
		ImGui::HelpMarker("再生する音源を指定します");

		ImGui::DragFloat("Time", &mTime);
		if (ImGui::DragFloat("ScrollSpeed", &mScrollSpeed, 0.1f, 1.0f, 20.0f)) {
			mCacheScrollPos.clear();
		}
		ImGui::Checkbox("EnableScrollChange", &mEnableScrollChangeInEditor);

		ImGui::InputInt("ノーツ配置間隔", &mSetLPB);
	}

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("ファイル")) {
			if (ImGui::BeginMenu("新規作成")) {
				if (ImGui::MenuItem("譜面ファイル(.kasu)")) {
					InitEditor();
					mState = EditorState::Chart;
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("開く")) {
				wchar_t filePath[MAX_PATH] = { 0 };

				OPENFILENAME ofn = { 0 };
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = RWindow::GetWindowHandle();
				ofn.lpstrFilter =
					L"譜面ファイル (.kasu)\0*.kasu\0"
					L"すべてのファイル (*.*)\0*.*\0";
				ofn.nFilterIndex = 0;
				ofn.lpstrFile = filePath;
				ofn.nMaxFile = MAX_PATH;

				auto old = std::filesystem::current_path();
				if (GetOpenFileName(&ofn)) {
					InitEditor();
					std::wstring wStr = filePath;
					Load(wStr);
					mState = EditorState::Chart;
				}
				std::filesystem::current_path(old);
			}
			if (ImGui::MenuItem("保存")) {
				if (chartFile.path.empty()) {
					wchar_t filePath[MAX_PATH] = { 0 };

					OPENFILENAME ofn = { 0 };
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = RWindow::GetWindowHandle();
					ofn.lpstrFilter =
						L"譜面ファイル (.kasu)\0*.kasu\0";
					ofn.lpstrDefExt = L"kasu";
					ofn.nFilterIndex = 0;
					ofn.lpstrFile = filePath;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_OVERWRITEPROMPT;

					auto old = std::filesystem::current_path();
					if (GetSaveFileName(&ofn)) {
						std::wstring wStr = filePath;
						Save(wStr);
					}
					std::filesystem::current_path(old);
				}
				else {
					Save(Util::ConvertStringToWString(chartFile.path));
				}
			}
			if (ImGui::MenuItem("名前を付けて保存")) {
				wchar_t filePath[MAX_PATH] = { 0 };

				OPENFILENAME ofn = { 0 };
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = RWindow::GetWindowHandle();
				ofn.lpstrFilter =
					L"譜面ファイル (.kasu)\0*.kasu\0";
				ofn.lpstrDefExt = L"kasu";
				ofn.nFilterIndex = 0;
				ofn.lpstrFile = filePath;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_OVERWRITEPROMPT;

				auto old = std::filesystem::current_path();
				if (GetSaveFileName(&ofn)) {
					std::wstring wStr = filePath;
					Save(wStr);
				}
				std::filesystem::current_path(old);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void EditorScene::ShowToolWindow()
{
	ImGui::SetNextWindowPos({ 1280, 0 }, ImGuiCond_FirstUseEver, {1, 0});

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("ツール", NULL, window_flags);

	if (ImGui::RadioButton("ペン", mMode == EditMode::Pen)) {
		mMode = EditMode::Pen;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("消しゴム", mMode == EditMode::Erase)) {
		mMode = EditMode::Erase;
	}

	if (mMode == EditMode::Pen) {
		if (ImGui::RadioButton("タップ", mEditType == EditType::TapNote)) {
			mEditType = EditType::TapNote;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("レーザー", mEditType == EditType::ArcNote)) {
			mEditType = EditType::ArcNote;
		}
	}

	ImGui::End();
}

float EditorScene::GetScroll(float miliSec)
{
	std::pair<Beat, float> nowScroll({ 0, 0, 1 }, 1.0f);

	for (std::pair<Beat, float> change : chartFile.scrollChange) {
		if (miliSec < chartFile.music.ConvertBeatToMiliSeconds(change.first)) {
			break;
		}

		nowScroll = change;
	}

	return nowScroll.second;
}

float EditorScene::GetPosition(float miliSec)
{
	float base = 0.2f * mScrollSpeed;
	float pos = 0;

	std::pair<Beat, float> nowScroll({ 0, 0, 1 }, 1.0f);

	if (mEnableScrollChangeInEditor) {
		for (std::pair<Beat, float> change : chartFile.scrollChange) {
			if (miliSec < chartFile.music.ConvertBeatToMiliSeconds(change.first)) {
				break;
			}

			auto cache = mCacheScrollPos.find(change.first);
			if (cache == mCacheScrollPos.end()) {
				pos += (base * nowScroll.second) * (chartFile.music.ConvertBeatToMiliSeconds(change.first) - chartFile.music.ConvertBeatToMiliSeconds(nowScroll.first));
				mCacheScrollPos[change.first] = pos;
			}
			else {
				pos = (*cache).second;
			}

			nowScroll = change;
		}
	}

	pos += (base * nowScroll.second) * (miliSec - chartFile.music.ConvertBeatToMiliSeconds(nowScroll.first));
	return pos;
}

Beat EditorScene::GetCursorBeat()
{
	Beat cursorBeat = { chartFile.music.ConvertMiliSecondsToBeat(mTime).measure, 0, mSetLPB };
	float cursorDistance = abs(RInput::GetMousePos().y - GetScreenPosition(cursorBeat));

	bool isStartUpOut = GetScreenPosition(cursorBeat) < 0;
	bool isStartDownOut = GetScreenPosition(cursorBeat) > 720;

	bool checkInside = true;
	int32_t startMeasure = cursorBeat.measure;
	Beat checkBeat = cursorBeat;
	if (!isStartUpOut) {
		if (isStartDownOut) {
			checkInside = false;
		}
		while (true) {
			checkBeat.beat++;
			if (checkBeat.beat >= mSetLPB - 1) {
				checkBeat.beat = 0;
				checkBeat.measure++;
			}

			if (abs(checkBeat.measure - startMeasure) >= 100) {
				break;
			}

			float pos = GetScreenPosition(checkBeat);

			if (checkInside && pos < 0) {
				break;
			}
			else {
				checkInside = true;
			}

			float distance = abs(RInput::GetMousePos().y - pos);
			if (distance < cursorDistance) {
				cursorBeat = checkBeat;
				cursorDistance = distance;
			}
			else {
				break;
			}
		}
	}
	if (!isStartDownOut) {
		if (isStartUpOut) {
			checkInside = false;
		}
		while (true) {
			checkBeat.beat--;
			if (checkBeat.beat < 0) {
				checkBeat.beat = mSetLPB - 1;
				checkBeat.measure--;
			}

			if (abs(checkBeat.measure - startMeasure) >= 100) {
				break;
			}

			float pos = GetScreenPosition(checkBeat);

			if (checkInside && pos > 720) {
				break;
			}
			else {
				checkInside = true;
			}

			float distance = abs(RInput::GetMousePos().y - pos);
			if (distance <= cursorDistance) {
				cursorBeat = checkBeat;
				cursorDistance = distance;
			}
			else {
				break;
			}
		}
	}
	return cursorBeat;
}

void EditorScene::DrawMeasureLine(Beat beat, Color color, bool drawNum)
{
	float posY = sBaseLineY - (GetPosition(chartFile.music.ConvertBeatToMiliSeconds(beat)) - mNowScrollPos);

	if (posY < 0 || posY > 720) {
		return;
	}

	SimpleDrawer::DrawLine(sMinLaneX, static_cast<int32_t>(posY), sMaxLaneX, static_cast<int32_t>(posY), 0, color, 2);
	if(drawNum) SimpleDrawer::DrawString(sMinLaneX - 20.0f, posY, 0, Util::StringFormat("%d", beat.measure), color, "", 20, { 1.0f, 0.5f });
}

void EditorScene::DrawMeasureLines()
{
	Beat nowBeat = chartFile.music.ConvertMiliSecondsToBeat(mTime);
	for (int32_t measure = nowBeat.measure; ;) {
		if (abs(nowBeat.measure - measure) > 100) {
			break;
		}

		DrawMeasureLine({ measure, 0, 1 }, Color(1, 1, 1, 1), true);

		float nextMeasureTime = chartFile.music.ConvertBeatToMiliSeconds({ measure + 1, 0, 1 });
		for (int32_t beat = 1; beat < mSetLPB; beat++) {
			if (chartFile.music.ConvertBeatToMiliSeconds({ measure, beat, mSetLPB })
				>= nextMeasureTime) {
				break;
			}
			DrawMeasureLine({ measure, beat, mSetLPB }, Color(1, 1, 1, 0.1f), false);
		}
		measure--;
	}
	for (int32_t measure = nowBeat.measure + 1; ;) {
		if (abs(nowBeat.measure - measure) > 100) {
			break;
		}

		DrawMeasureLine({ measure, 0, 1 }, Color(1, 1, 1, 1), true);

		float nextMeasureTime = chartFile.music.ConvertBeatToMiliSeconds({ measure + 1, 0, 1 });
		for (int32_t beat = 1; beat < mSetLPB; beat++) {
			if (chartFile.music.ConvertBeatToMiliSeconds({ measure, beat, mSetLPB })
				>= nextMeasureTime) {
				break;
			}
			DrawMeasureLine({ measure, beat, mSetLPB }, Color(1, 1, 1, 0.1f), false);
		}
		measure++;
	}

	for (std::pair<Beat, float> change : chartFile.scrollChange) {
		float posY = sBaseLineY - (GetPosition(chartFile.music.ConvertBeatToMiliSeconds(change.first)) - mNowScrollPos);

		if (posY < 0 || posY > 720) {
			continue;
		}

		SimpleDrawer::DrawLine(sMinLaneX, static_cast<int32_t>(posY), sMaxLaneX, static_cast<int32_t>(posY), 0, 0xaaffaa, 2);
		SimpleDrawer::DrawString(sMinLaneX - 60.0f, posY, 0, Util::StringFormat("x%.2f", change.second), 0xaaffaa, "", 20, { 1.0f, 0.5f });
	}
}

void EditorScene::DrawNotes()
{
	int32_t laneSize = sMaxLaneX - sMinLaneX;
	int32_t laneCenter = (sMinLaneX + sMaxLaneX) / 2;

	int32_t arcHeightSize = sMaxArcHeightX - sMinArcHeightX;

	for (Note& note : chartFile.notes) {
		if (note.type == Note::Types::Tap) {
			float x1 = sMinLaneX + laneSize / 4.0f * note.lane;
			float x2 = x1 + laneSize / 4.0f;

			float y = sBaseLineY - (GetPosition(chartFile.music.ConvertBeatToMiliSeconds(note.mainBeat)) - mNowScrollPos);

			if (y < 0 || y > 720) {
				continue;
			}

			SimpleDrawer::DrawBox(x1 + 4, y - 10, x2 - 4, y, 5, 0x00ffff, true);
		}
		else if (note.type == Note::Types::Arc) {
			Vector3 prevPos = note.mainPos;
			prevPos.z = GetPosition(chartFile.music.ConvertBeatToMiliSeconds(note.mainBeat));

			float startX = laneCenter + (prevPos.x / 8.0f) * (laneSize / 2);
			float startX2 = sMaxArcHeightX - (((prevPos.y - 1) / 4.0f) * arcHeightSize);
			float startZ = sBaseLineY - (prevPos.z - mNowScrollPos);
			SimpleDrawer::DrawBox(startX - 10, startZ - 10, startX + 10, startZ + 10, 10, 0xaa33aa, true);
			SimpleDrawer::DrawBox(startX2 - 10, startZ - 10, startX2 + 10, startZ + 10, 10, 0xaa33aa, true);

			Vector3 startPos;
			Vector3 endPos;

			auto itr = note.controlPoints.begin();

			while (true) {
				startPos = prevPos;
				if (itr != note.controlPoints.end()) {
					endPos = itr->pos;
					endPos.z = GetPosition(chartFile.music.ConvertBeatToMiliSeconds(itr->beat));
				}
				else {
					endPos = note.subPos;
					endPos.z = GetPosition(chartFile.music.ConvertBeatToMiliSeconds(note.subBeat));
				}

				startPos.z = sBaseLineY - (startPos.z - mNowScrollPos);
				endPos.z = sBaseLineY - (endPos.z - mNowScrollPos);

				if ((startPos.z < 0 && endPos.z < 0) || (startPos.z < 0 && endPos.z > 720)) {
					if (itr != note.controlPoints.end()) {
						prevPos = itr->pos;
						prevPos.z = GetPosition(chartFile.music.ConvertBeatToMiliSeconds(itr->beat));
						itr++;
						continue;
					}
					else {
						break;
					}
				}

				int32_t x1 = static_cast<int32_t>(laneCenter + (startPos.x / 8.0f) * (laneSize / 2));
				int32_t x2 = static_cast<int32_t>(laneCenter + (endPos.x / 8.0f) * (laneSize / 2));
				int32_t x3 = static_cast<int32_t>(sMaxArcHeightX - ((startPos.y - 1) / 4.0f) * arcHeightSize);
				int32_t x4 = static_cast<int32_t>(sMaxArcHeightX - ((endPos.y - 1) / 4.0f) * arcHeightSize);

				SimpleDrawer::DrawCircle(x2, static_cast<int32_t>(endPos.z), 12, 10, 0xaa33aa, true);
				SimpleDrawer::DrawLine(x1, static_cast<int32_t>(startPos.z), x2, static_cast<int32_t>(endPos.z), 10, 0xaa33aa, 10);
				SimpleDrawer::DrawCircle(x4, static_cast<int32_t>(endPos.z), 12, 10, 0xaa33aa, true);
				SimpleDrawer::DrawLine(x3, static_cast<int32_t>(startPos.z), x4, static_cast<int32_t>(endPos.z), 10, 0xaa33aa, 10);

				if (itr != note.controlPoints.end()) {
					prevPos = itr->pos;
					prevPos.z = GetPosition(chartFile.music.ConvertBeatToMiliSeconds(itr->beat));
					itr++;
				}
				else {
					break;
				}
			}
		}
	}
}

void EditorScene::InitEditor()
{
	mState = EditorState::None;
	chartFile = ChartFile();
	mEditorObjects.clear();
	mTime = 0;
}

void EditorScene::Load(std::wstring path)
{
	Init();
	chartFile.path = Util::ConvertWStringToString(path);
	chartFile.Load();

	for (auto& note : chartFile.notes) {
		if (note.type == Note::Types::Tap) {
			mEditorObjects.push_back(std::make_unique<EObjTapNote>(this));
			EObjTapNote* obj = static_cast<EObjTapNote*>(mEditorObjects.back().get());
			obj->mLane = note.lane;
			obj->mBeat = note.mainBeat;
		}
		else if (note.type == Note::Types::Arc) {
			mEditorObjects.push_back(std::make_unique<EObjArcNote>(this));
			EObjArcNote* obj = static_cast<EObjArcNote*>(mEditorObjects.back().get());
			obj->mStartBeat = note.mainBeat;
			obj->mEndBeat = note.subBeat;
			obj->mStartPos = note.mainPos;
			obj->mEndPos = note.subPos;

			for (Note::ControlPoint ncp : note.controlPoints) {
				EObjArcNote::ControlPoint cp;
				cp.beat = ncp.beat;
				cp.pos = ncp.pos;
				obj->mControlPoints.push_back(cp);
			}
		}
	}
}

void EditorScene::Save(std::wstring path)
{
	chartFile.path = Util::ConvertWStringToString(path);

	chartFile.notes.clear();
	for (auto& obj : mEditorObjects) {
		if (obj->GetTypeName() == "EObjTapNote") {
			EObjTapNote* note = static_cast<EObjTapNote*>(obj.get());
			Note chartNote;
			chartNote.type = Note::Types::Tap;
			chartNote.lane = note->mLane;
			chartNote.mainBeat = note->mBeat;
			chartFile.notes.push_back(chartNote);
		}
		else if (obj->GetTypeName() == "EObjArcNote") {
			EObjArcNote* note = static_cast<EObjArcNote*>(obj.get());
			Note chartNote;
			chartNote.type = Note::Types::Arc;
			chartNote.mainPos = note->mStartPos;
			chartNote.mainBeat = note->mStartBeat;
			chartNote.subPos = note->mEndPos;
			chartNote.subBeat = note->mEndBeat;
			for (auto& cp : note->mControlPoints) {
				Note::ControlPoint ncp;
				ncp.beat = cp.beat;
				ncp.pos = cp.pos;
				chartNote.controlPoints.push_back(ncp);
			}
			chartFile.notes.push_back(chartNote);
		}
	}
	
	chartFile.Save();
}
