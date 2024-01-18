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
#include <PathUtil.h>
#include <cuchar>
#include <SpectrumAnalyze.h>

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

	if ((RInput::GetKey(DIK_LCONTROL) || RInput::GetKey(DIK_RCONTROL)) && RInput::GetKeyDown(DIK_Z)) {
		Undo();
	}

	if ((RInput::GetKey(DIK_LCONTROL) || RInput::GetKey(DIK_RCONTROL)) && RInput::GetKeyDown(DIK_Y)) {
		Redo();
	}

	if (mState == EditorState::Chart) {
		if (GetActiveWindow() == RWindow::GetWindowHandle()
			&& !ImGui::GetIO().WantCaptureMouse) {

			if (RInput::GetMouseMove().z != 0)
				mTime += 5.0f * RInput::GetMouseMove().z;

			if (RInput::GetMouseClickDown(0)) {
				mDragStartPos = mouse;
			}

			if (RInput::GetMouseClickDown(1)) {
				mRightDragStartPos = mouse;
				mIsInsideMinimapDrag = mRightDragStartPos.x >= 1180;
				mIsRightDragging = true;
			}

			if (mIsRightDragging) {
				if (RInput::GetMouseClick(1)) {
					if (mIsInsideMinimapDrag) {
						mTime = GetMiliSecFromScreenLength(GetAllScreenLength() / 720 * (720 - mouse.y));
					}
					else {
						Vector2 diff = mouse - mRightDragStartPos;
						mRightDragStartPos = mouse;

						mTime += GetMiliSecFromScreenLength(diff.y);
					}
				}
				else {
					mIsRightDragging = false;
				}
			}

			if (RInput::GetKeyDown(DIK_DELETE)) {
				std::vector<std::unique_ptr<EditorAction>> acts;

				for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end();) {
					IEditorObject* obj = itr->get();
					if (obj->IsSelected()) {
						AddEditorAction(std::make_unique<EARemoveEO>(obj->mUuid, *itr));
						itr = mEditorObjects.erase(itr);
						continue;
					}

					itr++;
				}

				if(!acts.empty()) mEditorActions.push_back(std::move(acts));
			}

			if (mMode == EditMode::Pen || mMode == EditMode::AreaSelect) {
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
				if (mMode == EditMode::AreaSelect && (RInput::GetMouseClick(0) || RInput::GetMouseClickUp(0))) {
					if (RInput::GetMouseClickDown(0)) {
						mIsDragging = true;
						for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end(); itr++) {
							IEditorObject* obj = itr->get();
							if (obj->Collide(mDragStartPos.x, mDragStartPos.y)) {
								mIsDragging = false;
								break;
							}
						}
					}

					if (mIsDragging) {
						SimpleDrawer::DrawBox(mDragStartPos.x, mDragStartPos.y, mouse.x, mouse.y, 50, 0x00ffff, false, 2);

						if (RInput::GetMouseClickUp(0)) {
							for (auto itr = mEditorObjects.begin(); itr != mEditorObjects.end(); itr++) {
								IEditorObject* obj = itr->get();
								if (obj->Collide(mDragStartPos.x, mDragStartPos.y, mouse.x, mouse.y)) {
									checkOnObject = true;
									obj->Select(mDragStartPos.x, mDragStartPos.y, mouse.x, mouse.y, false);
								}
							}
						}
					}
				}
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

				//ドラッグ移動
				if (RInput::GetMouseClick(0)) {
					Vector2 drag = (mouse - mDragStartPos);
					float dragDistance = abs(drag.Length());

					std::vector<std::unique_ptr<EditorAction>> acts;

					for (IEditorObject* obj : selectList) {
						if (RInput::GetMouseClickDown(0)) {
							AddEditorAction(obj->GetSavePoint());
							obj->MoveStart();
						}
						if (dragDistance >= 5) {
							obj->Move(drag.x, drag.y);
						}
					}

					if(!acts.empty()) mEditorActions.push_back(std::move(acts));
				}

				//何も選択してないなら追加の処理
				if (mMode == EditMode::Pen) {
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
									AddEditorAction(std::make_unique<EAAddEO>(obj->mUuid, mEditorObjects.back()));
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
										AddEditorAction(std::make_unique<EAAddEO>(obj->mUuid, mEditorObjects.back()));
									}
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
								AddEditorAction(std::make_unique<EARemoveEO>(obj->mUuid, *itr));
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
									AddEditorAction(std::make_unique<EARemoveEO>(obj->mUuid, *itr));

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

	std::vector<std::unique_ptr<EditorAction>> acts;
	for (auto& act : mNowActions) {
		acts.push_back(std::move(act));
	}
	mNowActions.clear();
	if (!acts.empty()) {
		mFutureEditorActions.clear();
		mEditorActions.push_back(std::move(acts));
	}

	mNowScrollPos = GetPosition(mTime);
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

		float multipiler = 720 / GetAllScreenLength();
		for (std::unique_ptr<IEditorObject>& obj : mEditorObjects) {
			obj->Draw();
			obj->DrawToMiniMap();
		}

		SimpleDrawer::DrawLine(1180, 0, 1180, 720, 110, 0xffff00);

		float nowAreaY1 = 720 - (mNowScrollPos - (720 - sBaseLineY)) * multipiler;
		float nowAreaY2 = 720 - (mNowScrollPos + sBaseLineY) * multipiler;

		SimpleDrawer::DrawBox(1180.0f, nowAreaY1, 1280.0f, nowAreaY2, 115, 0x00ff33, false);
	}
}

float EditorScene::GetScreenPosition(Beat beat)
{
	return sBaseLineY - (GetPosition(chartFile.music.ConvertBeatToMiliSeconds(beat)) - mNowScrollPos);
}

float EditorScene::GetOriginScreenPosition(Beat beat)
{
	return GetPosition(chartFile.music.ConvertBeatToMiliSeconds(beat));
}

float EditorScene::GetMiliSecFromScreenLength(float length)
{
	float base = 0.2f * mScrollSpeed;
	return length / base;
}

float EditorScene::GetAllScreenLength()
{
	float musicLength = RAudio::GetLength(mAudioHandle) * 1000;
	if (musicLength == 0) {
		Beat save = { 0, 0, 1 };
		float saveSec = 0;
		for (auto& itr : mEditorObjects) {
			float tempSec = chartFile.music.ConvertBeatToMiliSeconds(itr->GetBeat());
			if (saveSec <= tempSec) {
				save = itr->GetBeat();
				saveSec = tempSec;
			}
		}

		//少なすぎると崩れるので最低5小節は入れる
		if (save < Beat(5, 0, 1)) {
			save = { 5, 0, 1 };
		}

		return GetOriginScreenPosition(save);
	}
	return GetPosition(musicLength);
}

Beat EditorScene::GetVirtualCursorBeat(float y)
{
	Beat cursorBeat = { chartFile.music.ConvertMiliSecondsToBeat(mTime).measure, 0, mSetLPB };
	float cursorDistance = abs(y - GetScreenPosition(cursorBeat));

	int32_t startMeasure = cursorBeat.measure;
	Beat checkBeat = cursorBeat;
	while (true) {
		checkBeat.beat++;
		if (chartFile.music.ConvertBeatToMiliSeconds(checkBeat) >= chartFile.music.ConvertBeatToMiliSeconds({ checkBeat.measure + 1, 0, 1 })) {
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
			checkBeat.beat = 0;
			checkBeat.measure--;
			while (chartFile.music.ConvertBeatToMiliSeconds({ checkBeat.measure, checkBeat.beat + 1, checkBeat.LPB }) < chartFile.music.ConvertBeatToMiliSeconds({ checkBeat.measure + 1, 0, 1 })) {
				checkBeat.beat++;
			}
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

void EditorScene::AddEditorAction(std::unique_ptr<EditorAction>&& act)
{
	mNowActions.push_back(std::move(act));
	mFutureEditorActions.clear();
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
	window_flags |= ImGuiWindowFlags_NoMove;
	ImGui::Begin("Editor", NULL, window_flags);

	if (mState == EditorState::Chart) {
		ImGui::Text("楽曲ファイル");
		if (ImGui::InputText("##楽曲ファイル", &chartFile.audiopath)) {
			std::filesystem::path base = PathUtil::ConvertAbsolute(chartFile.path);
			//std::u8string u8str = (const char8_t *)chartFile.audiopath.c_str();
			std::filesystem::path audio = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(chartFile.audiopath), base);
			std::string str = Util::ConvertWStringToString(audio.c_str());
			mAudioHandle = RAudio::Load(str);
		}
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
				std::filesystem::path path = PathUtil::ConvertRelativeFromTo(chartFile.path, filePath);
				chartFile.audiopath = Util::ConvertWStringToString(path.c_str());
				mAudioHandle = RAudio::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute(Util::ConvertStringToWString(chartFile.audiopath), PathUtil::ConvertAbsolute(chartFile.path)).c_str()));
			}
		}
		ImGui::SameLine();
		ImGui::HelpMarker("再生する音源を指定します");
		ImGui::Text("再生位置");
		float musicLength = RAudio::GetLength(mAudioHandle) * 1000;
		ImGui::SliderFloat("##再生位置スライダー", &mTime, 0, musicLength, (Util::GetTimeString(mTime / 1000) + " / " + Util::GetTimeString(musicLength / 1000)).c_str());
		ImGui::DragFloat("Time", &mTime);
		if (ImGui::DragFloat("ScrollSpeed", &mScrollSpeed, 0.1f, 1.0f, 20.0f)) {
			mCacheScrollPos.clear();
		}

		ImGui::Text("ノーツ配置間隔");
		ImGui::InputInt("##ノーツ配置間隔入力", &mSetLPB);
		ImGui::SameLine();
		ImGui::HelpMarker("ノーツを配置する際の間隔を何分音符区切りにするか決定します");

		ImGui::Text("BPM");
		ImGui::InputFloat("##BPM入力", &chartFile.music.baseBPM);
		ImGui::SameLine();
		if (ImGui::Button("...##BPM変化開")) {
			mOpenBPMChangeWindow = true;
		}
		ImGui::SameLine();
		ImGui::HelpMarker("楽曲の基本BPM(テンポ)を設定します");
		
		ImGui::Text("拍子");
		ImGui::PushItemWidth(50);
		ImGui::InputInt("##拍子入力", &chartFile.music.baseMeter.beatAmount, 0, 0);
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		ImGui::InputInt("##拍子入力2", &chartFile.music.baseMeter.beatLength, 0);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("...##拍子変化開")) {
			mOpenMeterChangeWindow = true;
		}
		ImGui::SameLine();
		ImGui::HelpMarker("楽曲の基本拍子(1小節の長さ)を設定します");
	}

	ShowMainWindowMenuBar();
	if (mOpenBPMChangeWindow) ShowBPMChangeWindow();
	if (mOpenMeterChangeWindow) ShowMeterChangeWindow();
	if (mOpenAutoGenerateWindow) ShowAutoGenerateWindow();

	ImGui::End();
}

void EditorScene::ShowMainWindowMenuBar()
{
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
					Save(chartFile.path);
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
		if (ImGui::BeginMenu("編集")) {
			if (ImGui::MenuItem("元に戻す  (Ctrl + Z)")) {
				Undo();
			}
			if (ImGui::MenuItem("やり直し  (Ctrl + Y)")) {
				Redo();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("譜面の自動生成")) {
				mOpenAutoGenerateWindow = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}

void EditorScene::ShowToolWindow()
{
	if (mState == EditorState::Chart) {
		ImGui::SetNextWindowPos({ 1280, 0 }, ImGuiCond_FirstUseEver, { 1, 0 });

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
		ImGui::SameLine();
		if (ImGui::RadioButton("範囲選択", mMode == EditMode::AreaSelect)) {
			mMode = EditMode::AreaSelect;
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
}

void EditorScene::ShowBPMChangeWindow() {
	ImGui::SetNextWindowPos({ 640, 360 }, ImGuiCond_FirstUseEver, { 0.5f, 0.5f });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("BPM変化", &mOpenBPMChangeWindow, window_flags);
	if (ImGui::Button("追加##BPM変化追加")) {
		mBPMChanges.push_back({ {0, 0, 1}, 120 });
	}
	ImGui::BeginChild("##BPM変化の子", { 600, 300 }, true);
	int32_t i = 0;
	bool checkActive = false;
	for (auto itr = mBPMChanges.begin(); itr != mBPMChanges.end();) {
		EBPMChange& c = *itr;

		ImGui::PushItemWidth(50);
		ImGui::InputInt(Util::StringFormat("小節##小節%d", i).c_str(), &c.beat.measure, 0, 0);
		if (ImGui::IsItemActive()) checkActive = true;
		ImGui::SameLine();
		ImGui::BeginDisabled(c.beat.LPB == 1);
		ImGui::InputInt(Util::StringFormat("拍##拍%d", i).c_str(), &c.beat.beat, 0, 0);
		if (ImGui::IsItemActive()) checkActive = true;
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::InputInt(Util::StringFormat("基準音符##基準音符%d", i).c_str(), &c.beat.LPB, 0, 0)) {
			if (c.beat.LPB < 1) c.beat.LPB = 1;
		}
		if (ImGui::IsItemActive()) checkActive = true;
		ImGui::PopItemWidth();
		ImGui::SameLine(0, 20);
		ImGui::Text("BPM");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::InputFloat(Util::StringFormat("##BPM%d", i).c_str(), &c.bpm, 0, 0, "%.2f");
		if (ImGui::IsItemActive()) checkActive = true;
		ImGui::SameLine();
		if (ImGui::Button((Util::StringFormat("削除##%d", i).c_str()))) {
			i++;
			itr = mBPMChanges.erase(itr);
			continue;
		}
		i++;
		itr++;
	}

	if (!checkActive) {
		for (EBPMChange& c : mBPMChanges) {
			if (c.beat.LPB == 1) {
				c.beat.beat = 0;
			}
		}

		mBPMChanges.sort([](const EBPMChange& lhs, const EBPMChange& rhs) {
			return lhs.beat < rhs.beat;
		});
	}

	chartFile.music.bpmChange.clear();
	for (EBPMChange& c : mBPMChanges) {
		chartFile.music.bpmChange[c.beat] = c.bpm;
	}
	chartFile.music.cacheMiliSec.clear();
	mCacheScrollPos.clear();
	chartFile.music.Init();
	ImGui::EndChild();

	ImGui::End();
}

void EditorScene::ShowMeterChangeWindow() {
	ImGui::SetNextWindowPos({ 640, 360 }, ImGuiCond_FirstUseEver, { 0.5f, 0.5f });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("拍子変化", &mOpenMeterChangeWindow, window_flags);
	if (ImGui::Button("追加##拍子変化追加")) {
		mMeterChanges.push_back({ {0, 0, 1}, {4, 4} });
	}
	ImGui::BeginChild("##拍子変化リスト", { 600, 300 }, true);
	int32_t i = 0;
	bool checkActive = false;
	for (auto itr = mMeterChanges.begin(); itr != mMeterChanges.end();) {
		EMeterChange& c = *itr;

		ImGui::PushItemWidth(50);
		ImGui::InputInt(Util::StringFormat("小節##小節%d", i).c_str(), &c.beat.measure, 0, 0);
		if (ImGui::IsItemActive()) checkActive = true;
		ImGui::SameLine(0, 20);
		ImGui::InputInt(Util::StringFormat("拍数##拍数%d", i).c_str(), &c.meter.beatAmount, 0, 0);
		if (ImGui::IsItemActive()) checkActive = true;
		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();
		if (ImGui::InputInt(Util::StringFormat("基準音符##基準音符%d", i).c_str(), &c.meter.beatLength, 0, 0)) {
			if (c.meter.beatLength < 1) c.meter.beatLength = 1;
		}
		if (ImGui::IsItemActive()) checkActive = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button((Util::StringFormat("削除##%d", i).c_str()))) {
			i++;
			itr = mMeterChanges.erase(itr);
			continue;
		}
		i++;
		itr++;
	}

	if (!checkActive) {
		for (EMeterChange& c : mMeterChanges) {
			c.beat.beat = 0;
			c.beat.LPB = 1;

			if (c.meter.beatLength < 1) c.meter.beatLength = 1;
		}

		mMeterChanges.sort([](const EMeterChange& lhs, const EMeterChange& rhs) {
			return lhs.beat < rhs.beat;
		});
	}

	chartFile.music.meterChange.clear();
	for (EMeterChange& c : mMeterChanges) {
		chartFile.music.meterChange[c.beat] = c.meter;
	}
	chartFile.music.cacheMiliSec.clear();
	chartFile.music.Init();
	mCacheScrollPos.clear();
	ImGui::EndChild();

	ImGui::End();
}

void EditorScene::ShowAutoGenerateWindow() {
	ImGui::SetNextWindowPos({ 640, 360 }, ImGuiCond_FirstUseEver, { 0.5f, 0.5f });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("自動生成", &mOpenAutoGenerateWindow, window_flags);
	ImGui::Text("開発中機能です");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	int32_t startMeasure = chartFile.music.ConvertMiliSecondsToBeat(mTime).measure;

	ImGui::Text(Util::StringFormat("生成開始小節: %d小節目", startMeasure).c_str());
	static int32_t gMeasureSize = 1;
	ImGui::Text("生成する小節数: ");
	ImGui::SameLine();
	ImGui::InputInt("##生成小節数", &gMeasureSize);

	static float targetMin = 140;
	static float targetMax = 400;
	static float threshold = -30.0f;

	ImGui::Text("注目する周波数: ");
	ImGui::Text("下限: ");
	ImGui::SameLine();
	ImGui::DragFloat("##周波数A", &targetMin, 1.0f, 1.0f, targetMax);
	ImGui::Text("上限: ");
	ImGui::SameLine();
	ImGui::DragFloat("##周波数B", &targetMax, 1.0f, targetMin, 0.0f);
	ImGui::Text("しきい値: ");
	ImGui::SameLine();
	ImGui::SliderFloat("##周波数C", &threshold, -120.0f, 0.0f);

	if (mAudioHandle == "") ImGui::BeginDisabled();
	if (ImGui::Button("生成##自動生成開始")) {
		AutoGenerate(startMeasure, gMeasureSize, targetMin, targetMax, threshold);
	}
	if (mAudioHandle == "") ImGui::EndDisabled();

	ImGui::End();
}

void EditorScene::AutoGenerate(int32_t startMeasure, int32_t size, float targetMin, float targetMax, float threshold)
{
	startMeasure;
	size;
	targetMin;
	targetMax;

	if (mAudioHandle == "") return;
	AudioData* data = RAudio::GetData(mAudioHandle);
	if (data == nullptr) return;

	WaveAudio* audio = static_cast<WaveAudio*>(data);

	for (int32_t m = 0; m < size; m++) {
		for (int32_t b = 0; b < mSetLPB; b++) {
			Beat beat = { startMeasure + m, b, mSetLPB };

			std::vector<float> spectrum = SpectrumAnalyze(data, static_cast<int32_t>(chartFile.music.ConvertBeatToMiliSeconds(beat) / 1000.0f * audio->wfex.nSamplesPerSec), 2048);

			bool putFlag = false;

			for (int32_t i = 0; i < spectrum.size(); i++) {
				float magnitude = spectrum[i];
				float freq = i * static_cast<float>(audio->wfex.nSamplesPerSec) / 2048;
				if (freq >= targetMin && freq <= targetMax) {
					if (freq >= 0.0000000001f) {
						float logfreq = log(magnitude) * 10.0f;

						if (logfreq >= threshold) {
							putFlag = true;
							break;
						}
					}
				}
			}

			if (putFlag) {
				mEditorObjects.push_back(std::make_unique<EObjTapNote>(this));
				EObjTapNote* obj = static_cast<EObjTapNote*>(mEditorObjects.back().get());
				obj->mLane = Util::GetRand(0, 3);
				obj->mBeat = beat;
				AddEditorAction(std::make_unique<EAAddEO>(obj->mUuid, mEditorObjects.back()));
			}
		}
	}
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
	Beat cursorBeat = { chartFile.music.ConvertMiliSecondsToBeat(mTime).measure + 1, 0, mSetLPB };
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
			if (chartFile.music.ConvertBeatToMiliSeconds(checkBeat) >= chartFile.music.ConvertBeatToMiliSeconds({checkBeat.measure + 1, 0, 1})) {
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
				checkBeat.beat = 0;
				checkBeat.measure--;
				while (chartFile.music.ConvertBeatToMiliSeconds({checkBeat.measure, checkBeat.beat + 1, checkBeat.LPB}) < chartFile.music.ConvertBeatToMiliSeconds({ checkBeat.measure + 1, 0, 1 })) {
					checkBeat.beat++;
				}
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
	if (drawNum) SimpleDrawer::DrawString(sMinLaneX - 20.0f, posY, 0, Util::StringFormat("%d", beat.measure), color, "", 20, { 1.0f, 0.5f });
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
		for (int32_t beat = 1; true; beat++) {
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
		for (int32_t beat = 1; true; beat++) {
			if (chartFile.music.ConvertBeatToMiliSeconds({ measure, beat, mSetLPB })
				>= nextMeasureTime) {
				break;
			}
			DrawMeasureLine({ measure, beat, mSetLPB }, Color(1, 1, 1, 0.1f), false);
		}
		measure++;
	}

	for (std::pair<Beat, float> change : chartFile.music.bpmChange) {
		float posY = sBaseLineY - (GetPosition(chartFile.music.ConvertBeatToMiliSeconds(change.first)) - mNowScrollPos);

		if (posY < 0 || posY > 720) {
			continue;
		}

		SimpleDrawer::DrawString(sMaxLaneX + 20.0f, posY, 0, Util::StringFormat("%.2f", change.second), 0xffffaa, "", 20, { 0.0f, 0.5f });
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

void EditorScene::InitEditor()
{
	mState = EditorState::None;
	chartFile = ChartFile();
	chartFile.path = PathUtil::GetMainPath() / "Charts";
	mAudioHandle = "";
	mEditorObjects.clear();
	mTime = 0;
	mBPMChanges.clear();
	mMeterChanges.clear();
}

void EditorScene::Load(std::wstring path)
{
	chartFile.path = PathUtil::ConvertRelativeFromMainPath(path).c_str();
	chartFile.Load();

	mAudioHandle = RAudio::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute(Util::ConvertStringToWString(chartFile.audiopath), PathUtil::ConvertAbsolute(chartFile.path)).c_str()));

	for (auto& pair : chartFile.music.bpmChange) {
		mBPMChanges.push_back({ pair.first, pair.second });
	}

	for (auto& pair : chartFile.music.meterChange) {
		mMeterChanges.push_back({ pair.first, pair.second });
	}

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
	chartFile.path = PathUtil::ConvertRelativeFromMainPath(path).c_str();

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

void EditorScene::Undo()
{
	if (mEditorActions.empty()) return;

	std::vector<std::unique_ptr<EditorAction>> acts;

	for(auto itr = mEditorActions.back().rbegin(); itr != mEditorActions.back().rend(); itr++) {
		std::unique_ptr<EditorAction>& act = (*itr);
		act->Undo(&mEditorObjects);
		acts.push_back(std::move(act));
	}

	mEditorActions.pop_back();
	mFutureEditorActions.push_back(std::move(acts));
}

void EditorScene::Redo()
{
	if (mFutureEditorActions.empty()) return;

	std::vector<std::unique_ptr<EditorAction>> acts;
	for (auto itr = mFutureEditorActions.back().rbegin(); itr != mFutureEditorActions.back().rend(); itr++) {
		std::unique_ptr<EditorAction>& act = (*itr);
		act->Redo(&mEditorObjects);
		acts.push_back(std::move(act));
	}

	mFutureEditorActions.pop_back();
	mEditorActions.push_back(std::move(acts));
}
