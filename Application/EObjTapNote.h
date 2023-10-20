#pragma once
#include "IEditorObject.h"
#include <BeatUtil.h>
#include <Vector2.h>

class EObjTapNote : public IEditorObject
{
public:
	EObjTapNote(EditorScene* scene);
	std::string GetTypeName() override { return "EObjTapNote"; };

	int32_t mLane = 0;
	Beat mBeat;

	bool mIsSelected = false;
	Vector2 mMoveOrigin;
	
	bool Collide(float x, float y) override;
	bool Collide(float x1, float y1, float x2, float y2) override;

	bool IsSelected() override;
	int32_t GetSelectCountInThisInstance();
	void Select(float x, float y, bool keep) override;
	void Select(float x1, float y1, float x2, float y2, bool keep) override;
	void UnSelect() override;

	void MoveStart() override;
	void Move(float x, float y) override;

	void Draw() override;

	static void TempDraw(EditorScene* scene, int32_t lane, Beat beat);
};

