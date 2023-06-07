#include "RInput.h"
#include <memory>
#include <cassert>
#include "RWindow.h"

using namespace std;
using namespace Microsoft::WRL;

RInput* RInput::GetInstance() {
	static RInput instance;
	return &instance;
}

void RInput::Init() {
	GetInstance()->InitInternal();
}

void RInput::InitInternal()
{
	assert(!initialized);

	HRESULT result;

	result = DirectInput8Create(
		RWindow::GetWindowClassEx().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr
	);
	assert(SUCCEEDED(result));

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	result = mouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		RWindow::GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE // | DISCL_NOWINKEY //左から、アクティブウィンドウ,専有しない,Winキー無効
	);
	assert(SUCCEEDED(result));
	result = mouse->SetCooperativeLevel(
		RWindow::GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE //左から、アクティブウィンドウ,専有しない
	);
	assert(SUCCEEDED(result));

	ZeroMemory(&xInputState, sizeof(XINPUT_STATE));
	DWORD dresult;
	dresult = XInputGetState(0, &xInputState);

	if (result == ERROR_SUCCESS)
	{
		isConnectPad = true;
	}
	else
	{
		isConnectPad = false;
	}
}

void RInput::Update()
{
	RInput* instance = GetInstance();

	//情報の取得開始
	instance->keyboard->Acquire();
	instance->mouse->Acquire();

	//全キーの入力状態を取得する
	for (int i = 0; i < 256; i++) {
		instance->oldKeyState[i] = instance->keyState[i];
	}
	instance->oldMouseState = instance->mouseState;
	instance->keyboard->GetDeviceState(sizeof(keyState), instance->keyState);
	instance->mouse->GetDeviceState(sizeof(mouseState), &instance->mouseState);

	instance->oldMousePos = instance->mousePos;
	instance->mousePos = RWindow::GetInstance()->GetMousePos();

	instance->oldXInputState = instance->xInputState;
	DWORD dresult = XInputGetState(0, &instance->xInputState);
	if (dresult == ERROR_SUCCESS) {
		instance->isConnectPad = true;
	}
	else {
		instance->isConnectPad = false;
	}

	if ((instance->xInputState.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		instance->xInputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(instance->xInputState.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		instance->xInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		instance->xInputState.Gamepad.sThumbLX = 0;
		instance->xInputState.Gamepad.sThumbLY = 0;
	}

	if ((instance->xInputState.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		instance->xInputState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		(instance->xInputState.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		instance->xInputState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		instance->xInputState.Gamepad.sThumbRX = 0;
		instance->xInputState.Gamepad.sThumbRY = 0;
	}
}

bool RInput::GetKey(unsigned char key)
{
	return GetInstance()->keyState[key];
}

bool RInput::GetKeyUp(unsigned char key)
{
	return !GetInstance()->keyState[key] && GetInstance()->oldKeyState[key];
}

bool RInput::GetKeyDown(unsigned char key)
{
	return GetInstance()->keyState[key] && !GetInstance()->oldKeyState[key];
}

bool RInput::GetMouseClick(int buttonNum)
{
	return (GetInstance()->mouseState.rgbButtons[buttonNum] & 0x80) != 0;
}

bool RInput::GetMouseClickUp(int buttonNum)
{
	return (GetInstance()->mouseState.rgbButtons[buttonNum] & 0x80) == 0 && (GetInstance()->oldMouseState.rgbButtons[buttonNum] & 0x80) != 0;
}

bool RInput::GetMouseClickDown(int buttonNum)
{
	return (GetInstance()->mouseState.rgbButtons[buttonNum] & 0x80) != 0 && (GetInstance()->oldMouseState.rgbButtons[buttonNum] & 0x80) == 0;
}

Vector2 RInput::GetMousePos()
{
	return GetInstance()->mousePos;
}

Vector2 RInput::GetOldMousePos()
{
	return GetInstance()->oldMousePos;
}

Vector3 RInput::GetMouseMove() {
	return Vector3((float)GetInstance()->mouseState.lX, (float)GetInstance()->mouseState.lY, (float)GetInstance()->mouseState.lZ);
}

bool RInput::GetPadConnect()
{
	return GetInstance()->isConnectPad;
}

bool RInput::GetPadButton(UINT button)
{
	return GetInstance()->xInputState.Gamepad.wButtons == button;
}

bool RInput::GetPadButtonUp(UINT button)
{
	return GetInstance()->xInputState.Gamepad.wButtons != button && GetInstance()->oldXInputState.Gamepad.wButtons == button;
}

bool RInput::GetPadButtonDown(UINT button)
{
	return GetInstance()->xInputState.Gamepad.wButtons == button && GetInstance()->oldXInputState.Gamepad.wButtons != button;
}

Vector2 RInput::GetPadLStick()
{
	SHORT x = xInputState.Gamepad.sThumbLX;
	SHORT y = xInputState.Gamepad.sThumbLY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

Vector2 RInput::GetPadRStick()
{
	SHORT x = xInputState.Gamepad.sThumbRX;
	SHORT y = xInputState.Gamepad.sThumbRY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

bool RInput::GetLTriggerDown()
{
	if (oldXInputState.Gamepad.bLeftTrigger < 128 && xInputState.Gamepad.bLeftTrigger >= 128)
	{
		return true;
	}
	return false;
}

bool RInput::GetRTriggerDown()
{
	if (oldXInputState.Gamepad.bRightTrigger < 128 && xInputState.Gamepad.bRightTrigger >= 128)
	{
		return true;
	}
	return false;
}

bool RInput::GetLStickUp()
{
	if (oldXInputState.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		xInputState.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return true;
	}
	return false;
}

bool RInput::GetLStickDown()
{
	if (oldXInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		xInputState.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return true;
	}
	return false;
}

Vector2 RInput::GetLStick(bool useWASD, bool useArrow)
{
	RInput* instance = GetInstance();

	Vector2 pad;
	pad.x = static_cast<float>(instance->xInputState.Gamepad.sThumbLX) / 32767.0f;
	pad.y = static_cast<float>(instance->xInputState.Gamepad.sThumbLY) / 32767.0f;

	Vector2 wasd;
	if (useWASD) {
		wasd.x = static_cast<float>(instance->keyState[DIK_D] - instance->keyState[DIK_A]);
		wasd.y = static_cast<float>(instance->keyState[DIK_W] - instance->keyState[DIK_S]);
	}

	Vector2 arrow;
	if (useArrow) {
		arrow.x = static_cast<float>(instance->keyState[DIK_RIGHT] - instance->keyState[DIK_LEFT]);
		arrow.y = static_cast<float>(instance->keyState[DIK_UP] - instance->keyState[DIK_DOWN]);
	}

	Vector2 result = pad + wasd + arrow;
	result.Normalize();
	return result;
}

Vector2 RInput::GetRStick(bool useWASD, bool useArrow)
{
	RInput* instance = GetInstance();

	Vector2 pad;
	pad.x = static_cast<float>(instance->xInputState.Gamepad.sThumbRX) / 32767.0f;
	pad.y = static_cast<float>(instance->xInputState.Gamepad.sThumbRY) / 32767.0f;

	Vector2 wasd;
	if (useWASD) {
		wasd.x = static_cast<float>(instance->keyState[DIK_D] - instance->keyState[DIK_A]);
		wasd.y = static_cast<float>(instance->keyState[DIK_W] - instance->keyState[DIK_S]);
	}

	Vector2 arrow;
	if (useArrow) {
		arrow.x = static_cast<float>(instance->keyState[DIK_RIGHT] - instance->keyState[DIK_LEFT]);
		arrow.y = static_cast<float>(instance->keyState[DIK_UP] - instance->keyState[DIK_DOWN]);
	}

	Vector2 result = pad + wasd + arrow;
	result.Normalize();
	return result;
}
