#pragma once
#include <string>

class EditorScene;

class IEditorObject
{
public:
	EditorScene* mScene;
	std::string mUuid;
	bool mShowDetailWindow = false;

	IEditorObject(EditorScene* scene);
	virtual ~IEditorObject() {};

	virtual std::string GetTypeName() = 0;

	virtual bool Collide(float x, float y) { x; y; return false; };
	virtual bool Collide(float x1, float y1, float x2, float y2) { x1; y1; x2; y2; return false; };

	virtual bool IsSelected() { return false; };
	virtual int32_t GetSelectCountInThisInstance() { return 0; }
	virtual void Select(float x, float y, bool keep) { x; y; keep; };
	virtual void Select(float x1, float y1, float x2, float y2, bool keep) { x1; y1; x2; y2; keep; };
	virtual void UnSelect() {};

	virtual void OnErase() {};

	virtual void Update() {};
	virtual void SelectUpdate() {};

	virtual void MoveStart() {};
	virtual void Move(float x, float y) { x; y; };

	virtual void Draw() {};
	virtual void SelectDraw() {};
};

