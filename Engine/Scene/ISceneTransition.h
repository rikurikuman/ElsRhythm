#pragma once
class ISceneTransition
{
protected:
	bool closeFlag = false;

public:
	virtual ~ISceneTransition() {}

	virtual void Update() {};
	virtual void Draw() {};

	//トランジションを開ける
	virtual void Open() { closeFlag = false; };
	//トランジションが開ききっているか
	virtual bool IsOpened() { return !closeFlag; };
	//トランジションを閉める
	virtual void Close() { closeFlag = true; };
	//トランジションが閉まりきっているか
	virtual bool IsClosed() { return closeFlag; };
	//トランジションが動作中か
	virtual bool InProgress() { return false; };
};