#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include "Vector3.h"
#include "Vector2.h"
#include <Xinput.h>

class RInput {
public:
	//���͏��Ǘ��@�\������
	static void Init();
	static RInput* GetInstance();

	//���͏��̍X�V
	static void Update();

	//�L�[��������Ă��邩
	static bool GetKey(unsigned char key);
	//�L�[�����ꂽ�u�Ԃ�
	static bool GetKeyUp(unsigned char key);
	//�L�[�������ꂽ�u�Ԃ�
	static bool GetKeyDown(unsigned char key);

	//�}�E�X�̏�Ԃ𓾂�
	static inline DIMOUSESTATE2 GetMouseState() {
		return GetInstance()->mMouseState;
	}
	//�O��̃}�E�X�̏�Ԃ𓾂�
	static inline DIMOUSESTATE2 GetOldMouseState() {
		return GetInstance()->mOldMouseState;
	}

	//�}�E�X�N���b�N����Ă��邩
	static bool GetMouseClick(int32_t buttonNum);
	//�}�E�X�N���b�N�����ꂽ�u�Ԃ�
	static bool GetMouseClickUp(int32_t buttonNum);
	//�}�E�X�N���b�N�����ꂽ�u�Ԃ�
	static bool GetMouseClickDown(int32_t buttonNum);

	//�}�E�X�̈ʒu���擾����
	static Vector2 GetMousePos();

	//�O��̃}�E�X�̈ʒu���擾����
	static Vector2 GetOldMousePos();

	//�}�E�X�̈ړ��ʂ��擾����
	static Vector3 GetMouseMove();

	//�p�b�h�ɐڑ�����Ă邩
	static bool GetPadConnect();

	//�p�b�h�̃{�^����������Ă��邩
	static bool GetPadButton(uint32_t button);

	//�p�b�h�̃{�^�������ꂽ�u�Ԃ�
	static bool GetPadButtonUp(uint32_t button);

	//�p�b�h�̃{�^���������ꂽ�u�Ԃ�
	static bool GetPadButtonDown(uint32_t button);

	//�p�b�h�̍��X�e�B�b�N
	Vector2 GetPadLStick();

	//�p�b�h�̉E�X�e�B�b�N
	Vector2 GetPadRStick();

	//���g���K�[���������񂾏u�Ԃ�
	bool GetLTriggerDown();

	//�E�g���K�[���������񂾏u�Ԃ�
	bool GetRTriggerDown();

	//���X�e�B�b�N����ɓ|�����u�Ԃ�
	bool GetLStickUp();

	//���X�e�B�b�N�����ɓ|�����u�Ԃ�
	bool GetLStickDown();

	/// <summary>
	/// ���X�e�B�b�N���͂𓾂�
	/// </summary>
	/// <param name="useWASD">WASD�L�[���X�e�B�b�N�Ɍ����Ăď�������</param>
	/// <param name="useArrow">���L�[���X�e�B�b�N�Ɍ����Ăď�������</param>
	/// <returns>���͗�</returns>
	static Vector2 GetLStick(bool useWASD, bool useArrow);

	/// <summary>
	/// �E�X�e�B�b�N���͂𓾂�
	/// </summary>
	/// <param name="useWASD">WASD�L�[���X�e�B�b�N�Ɍ����Ăď�������</param>
	/// <param name="useArrow">���L�[���X�e�B�b�N�Ɍ����Ăď�������</param>
	/// <returns>���͗�</returns>
	static Vector2 GetRStick(bool useWASD, bool useArrow);

private:
	RInput() {};
	~RInput() = default;
	RInput(const RInput&) {};
	RInput& operator=(const RInput&) { return *this; }

	bool mIsInitialized = false;
	Microsoft::WRL::ComPtr<IDirectInput8> mDirectInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mKeyboard = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mMouse = nullptr;
	BYTE mKeyState[256] = {};
	BYTE mOldKeyState[256] = {};
	DIMOUSESTATE2 mMouseState = {};
	DIMOUSESTATE2 mOldMouseState = {};
	Vector2 mMousePos;
	Vector2 mOldMousePos;
	XINPUT_STATE mXInputState{};
	XINPUT_STATE mOldXInputState{};
	bool mIsPadConnected = false;

	void InitInternal();
};