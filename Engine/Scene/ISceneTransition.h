#pragma once
class ISceneTransition
{
protected:
	bool closeFlag = false;

public:
	virtual ~ISceneTransition() {}

	virtual void Update() {};
	virtual void Draw() {};

	//�g�����W�V�������J����
	virtual void Open() { closeFlag = false; };
	//�g�����W�V�������J�������Ă��邩
	virtual bool IsOpened() { return !closeFlag; };
	//�g�����W�V������߂�
	virtual void Close() { closeFlag = true; };
	//�g�����W�V�������܂肫���Ă��邩
	virtual bool IsClosed() { return closeFlag; };
	//�g�����W�V���������쒆��
	virtual bool InProgress() { return false; };
};