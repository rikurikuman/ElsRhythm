#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include "Vector2.h"

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class RWindow
{
private:
	RWindow() {};
	~RWindow() = default;
	RWindow(const RWindow&) {};
	RWindow& operator=(const RWindow&) { return *this; }

	LRESULT(*WindowProc)(HWND, UINT, WPARAM, LPARAM) = _DefWindowProc; //�E�B���h�E�v���V�[�W���ւ̃|�C���^

	int32_t mWindowWidth = 1280; //�E�B���h�E�̕�
	int32_t mWindowHeight = 720; //�E�B���h�E�̍���
	std::wstring mWindowName = L"DirectXGame";

	uint32_t mIconID = 0;

	WNDCLASSEX mWndClassEx{};
	HWND mWndHandle{};
	MSG mMsg{};

	bool mMouseCursorLock = false;

	void InitInternal();

public:
	//WindowsAPI�̏���������
	static void Init();
	static RWindow* GetInstance();

	static void SetWindowSize(const int32_t width, const int32_t height) {
		GetInstance()->mWindowWidth = width;
		GetInstance()->mWindowHeight = height;
	}

	static void SetWindowName(const std::wstring name) {
		GetInstance()->mWindowName = name;
	}

	static void SetIcon(const int32_t id) {
		GetInstance()->mIconID = id;
	}

	static int32_t GetWidth() {
		return GetInstance()->mWindowWidth;
	}

	static int32_t GetHeight() {
		return GetInstance()->mWindowHeight;
	}

	static WNDCLASSEX GetWindowClassEx() {
		return GetInstance()->mWndClassEx;
	}

	static HWND GetWindowHandle() {
		return GetInstance()->mWndHandle;
	}

	static MSG GetMessageStructure() {
		return GetInstance()->mMsg;
	}

	//���b�Z�[�W����
	static void ProcessMessage();

	static Vector2 GetMousePos();

	/// <summary>
	/// �}�E�X�J�[�\���ړ�
	/// �E�B���h�E�̍����0, 0�Ƃ��Ďw�肷��
	/// </summary>
	/// <param name="posX">�ړ���X���W</param>
	/// <param name="posY">�ړ���Y���W</param>
	static void SetMousePos(int32_t posX, int32_t posY);

	/// <summary>
	/// �}�E�X�J�[�\���ړ�
	/// ���j�^�[�̍����0, 0�Ƃ��Ďw�肷��
	/// </summary>
	/// <param name="posX">�ړ���X���W</param>
	/// <param name="posY">�ړ���Y���W</param>
	static void SetAbsMousePos(int32_t posX, int32_t posY);

	static void SetMouseHideFlag(bool hide);

	/// <summary>
	/// �}�E�X�J�[�\���̃��b�N
	/// ��\���ɂ��������ŉ�ʒ����ɌŒ肵�܂�
	/// </summary>
	/// <param name="lock">���b�N���邩�ǂ���</param>
	static void SetMouseLock(bool lock) {
		GetInstance()->mMouseCursorLock = lock;
	}
};