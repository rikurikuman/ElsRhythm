#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include "Vector3.h"
#include "Vector2.h"
#include <Xinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

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
		return GetInstance()->mouseState;
	}
	//�O��̃}�E�X�̏�Ԃ𓾂�
	static inline DIMOUSESTATE2 GetOldMouseState() {
		return GetInstance()->oldMouseState;
	}

	//�}�E�X�N���b�N����Ă��邩
	static bool GetMouseClick(int buttonNum);
	//�}�E�X�N���b�N�����ꂽ�u�Ԃ�
	static bool GetMouseClickUp(int buttonNum);
	//�}�E�X�N���b�N�����ꂽ�u�Ԃ�
	static bool GetMouseClickDown(int buttonNum);

	//�}�E�X�̈ʒu���擾����
	static Vector2 GetMousePos();

	//�O��̃}�E�X�̈ʒu���擾����
	static Vector2 GetOldMousePos();

	//�}�E�X�̈ړ��ʂ��擾����
	static Vector3 GetMouseMove();

	//�p�b�h�ɐڑ�����Ă邩
	static bool GetPadConnect();

	//�p�b�h�̃{�^����������Ă��邩
	static bool GetPadButton(UINT button);

	//�p�b�h�̃{�^�������ꂽ�u�Ԃ�
	static bool GetPadButtonUp(UINT button);

	//�p�b�h�̃{�^���������ꂽ�u�Ԃ�
	static bool GetPadButtonDown(UINT button);

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
	RInput(const RInput& a) {};
	RInput& operator=(const RInput&) { return *this; }

	bool initialized = false;
	Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse = nullptr;
	BYTE keyState[256] = {};
	BYTE oldKeyState[256] = {};
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 oldMouseState = {};
	Vector2 mousePos;
	Vector2 oldMousePos;
	XINPUT_STATE xInputState;
	XINPUT_STATE oldXInputState;
	bool isConnectPad = false;

	void InitInternal();
};