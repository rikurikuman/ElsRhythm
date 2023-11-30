#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "Image3D.h"
#include "SRConstBuffer.h"
#include <ChartFile.h>
#include <IEditorObject.h>
#include "RAudio.h"
#include <EditorAction.h>

class EditorScene : public IScene
{
public:
	EditorScene();

	void Init() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

	constexpr static int32_t sBaseLineY = 625;
	constexpr static int32_t sMinFieldX = 0;
	constexpr static int32_t sMaxFieldX = 1280;
	constexpr static int32_t sMinLaneX = 750;
	constexpr static int32_t sMaxLaneX = 1150;
	constexpr static int32_t sMinArcHeightX = 200;
	constexpr static int32_t sMaxArcHeightX = 560;

	float GetScreenPosition(Beat beat);
	Beat GetVirtualCursorBeat(float y);

	void AddEditorAction(std::unique_ptr<EditorAction>&& act);

private:
	DebugCamera camera = DebugCamera({0, 0, -5});
	LightGroup light;

	ModelObj mJudgeLineObj;
	ModelObj mLaneObj;
	std::array<ModelObj, 3> mLineObjs;

	enum class EditorState {
		None,
		Chart,
	};
	enum class EditMode {
		Pen,
		Erase,
		AreaSelect,
	};
	enum class EditType {
		TapNote,
		ArcNote,
	};
	EditorState mState = EditorState::None;
	EditMode mMode = EditMode::Pen;
	EditType mEditType = EditType::TapNote;

	ChartFile chartFile;
	AudioHandle mAudioHandle;

	std::list<std::unique_ptr<IEditorObject>> mEditorObjects;
	std::list<std::unique_ptr<EditorAction>> mNowActions; //このフレームに行われたアクション
	std::list<std::vector<std::unique_ptr<EditorAction>>> mEditorActions; //今まで行われたアクション
	std::list<std::vector<std::unique_ptr<EditorAction>>> mFutureEditorActions; //Undoによって戻された、これからのアクション

	float mTime = 0;
	float mNowScrollPos = 0;

	bool misDragging = false;
	Vector2 mDragStartPos;

	bool mEnableScrollChangeInEditor = false;
	float mScrollSpeed = 1.0f;
	std::map<Beat, float> mCacheScrollPos;

	int32_t mSetLPB = 4;

	void ShowWindow();
	void ShowMainWindow();
	void ShowMainWindowMenuBar();
	void ShowToolWindow();

	struct EBPMChange {
		Beat beat;
		float bpm;
	};
	struct EMeterChange {
		Beat beat;
		Meter meter;
	};
	std::list<EBPMChange> mBPMChanges;
	bool mOpenBPMChangeWindow = false;
	void ShowBPMChangeWindow();
	std::list<EMeterChange> mMeterChanges;
	bool mOpenMeterChangeWindow = false;
	void ShowMeterChangeWindow();

	float GetScroll(float miliSec);
	float GetPosition(float miliSec);
	Beat GetCursorBeat();
	void DrawMeasureLine(Beat beat, Color color, bool drawNum);
	void DrawMeasureLines();
	void DrawNotes();

	void InitEditor();
	void Load(std::wstring path);
	void Save(std::wstring path);

	void Undo();
	void Redo();
};
