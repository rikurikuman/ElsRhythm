#include "RWindow.h"
#include <imgui_impl_win32.h>

using namespace std;

void RWindow::Init() {
	GetInstance()->InitInternal();
}

RWindow* RWindow::GetInstance() {
	static RWindow instance;
	return &instance;
}

void RWindow::InitInternal()
{
	mWndClassEx.cbSize = sizeof(WNDCLASSEX);
	mWndClassEx.lpfnWndProc = (WNDPROC)WindowProc;
	mWndClassEx.lpszClassName = mWindowName.c_str();
	mWndClassEx.hInstance = GetModuleHandle(nullptr);
	mWndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	if (mIconID != 0) {
		mWndClassEx.hIcon = LoadIcon(mWndClassEx.hInstance, MAKEINTRESOURCE(mIconID));
		mWndClassEx.hIconSm = LoadIcon(mWndClassEx.hInstance, MAKEINTRESOURCE(mIconID));
	}

	RegisterClassEx(&mWndClassEx);
	RECT wrc = { 0, 0, mWindowWidth, mWindowHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	mWndHandle = CreateWindow(mWndClassEx.lpszClassName,
		mWndClassEx.lpszClassName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr, //�e�E�B���h�E�n���h��
		nullptr, //���j���[�n���h��
		mWndClassEx.hInstance, //�Ăяo���A�v���P�[�V�����n���h��
		nullptr); //�I�v�V����

	ShowWindow(mWndHandle, SW_SHOW);
}

void RWindow::ProcessMessage()
{
	RWindow* instance = GetInstance();
	if (PeekMessage(&instance->mMsg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&instance->mMsg); //�L�[���̓��b�Z�[�W����
		DispatchMessage(&instance->mMsg);
	}
}

Vector2 RWindow::GetMousePos()
{
	POINT point;
	GetCursorPos(&point);

	WINDOWINFO windowInfo{};

	//�E�B���h�E�̈ʒu���擾
	windowInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(RWindow::GetWindowHandle(), &windowInfo);

	ScreenToClient(RWindow::GetWindowHandle(), &point);

	//�}�E�X�̈ړ���̐�΍��W�i���j�^�[���ォ��̍��W�j�ɂ��ĕԂ�
	return Vector2((float)point.x, (float)point.y);
}

void RWindow::SetMousePos(int32_t posX, int32_t posY)
{
	int32_t xPos_absolute, yPos_absolute;

	WINDOWINFO windowInfo{};

	//�E�B���h�E�̈ʒu���擾
	windowInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(RWindow::GetWindowHandle(), &windowInfo);

	//�}�E�X�̈ړ���̐�΍��W�i���j�^�[���ォ��̍��W�j
	xPos_absolute = posX + windowInfo.rcWindow.left;
	yPos_absolute = posY + windowInfo.rcWindow.top + 35; //�E�B���h�E�̃^�C�g���o�[�̕��i35px�j���v���X

	SetCursorPos(xPos_absolute, yPos_absolute);
}

void RWindow::SetAbsMousePos(int32_t posX, int32_t posY)
{
	SetCursorPos(posX, posY);
}

void RWindow::SetMouseHideFlag(bool hide)
{
	if (hide) {
		while (ShowCursor(0) >= 0);
	}
	else {
		while (ShowCursor(1) < 0);
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}