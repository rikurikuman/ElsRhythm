#pragma once
class ISceneTransition
{
protected:
	bool mIsClosed = false;

public:
	virtual ~ISceneTransition() {}

	virtual void Update() {};
	virtual void Draw() {};

	//�g�����W�V�������J����
	virtual void Open() { mIsClosed = false; };
	//�g�����W�V�������J�������Ă��邩
	virtual bool IsOpened() { return !mIsClosed; };
	//�g�����W�V������߂�
	virtual void Close() { mIsClosed = true; };
	//�g�����W�V�������܂肫���Ă��邩
	virtual bool IsClosed() { return mIsClosed; };
	//�g�����W�V���������쒆��
	virtual bool InProgress() { return false; };
};