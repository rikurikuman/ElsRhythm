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
	RWindow(const RWindow& a) {};
	RWindow& operator=(const RWindow&) { return *this; }

	LRESULT(*WindowProc)(HWND, UINT, WPARAM, LPARAM) = _DefWindowProc; //�E�B���h�E�v���V�[�W���ւ̃|�C���^

	int windowWidth = 1280; //�E�B���h�E�̕�
	int windowHeight = 720; //�E�B���h�E�̍���
	std::wstring windowName = L"DirectXGame";

	unsigned int iconID = 0;

	WNDCLASSEX wndClassEx{};
	HWND wndHandle;
	MSG msg{};

	void InitInternal();

public:
	//WindowsAPI�̏���������
	static void Init();
	static RWindow* GetInstance();

	static void SetWindowSize(const int width, const int height) {
		GetInstance()->windowWidth = width;
		GetInstance()->windowHeight = height;
	}

	static void SetWindowName(const std::wstring name) {
		GetInstance()->windowName = name;
	}

	static void SetIcon(const int id) {
		GetInstance()->iconID = id;
	}

	static int GetWidth() {
		return GetInstance()->windowWidth;
	}

	static int GetHeight() {
		return GetInstance()->windowHeight;
	}

	static WNDCLASSEX GetWindowClassEx() {
		return GetInstance()->wndClassEx;
	}

	static HWND GetWindowHandle() {
		return GetInstance()->wndHandle;
	}

	static MSG GetMessageStructure() {
		return GetInstance()->msg;
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
	static void SetMousePos(int posX, int posY);

	/// <summary>
	/// �}�E�X�J�[�\���ړ�
	/// ���j�^�[�̍����0, 0�Ƃ��Ďw�肷��
	/// </summary>
	/// <param name="posX">�ړ���X���W</param>
	/// <param name="posY">�ړ���Y���W</param>
	static void SetAbsMousePos(int posX, int posY);

	static void SetMouseHideFlag(bool hide);
};