#pragma once
#include <Windows.h>

class WinApp final
{
public://�萔
	//�E�B���h�D�T�C�Y
	static const int window_width = 1280; //����
	static const int window_height = 720; //�c��

public://�����o�֐�
	
	//�E�B���h�D�v���V�[�W��
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//������
	void Initialize();

	//�X�V
	bool ProcessMessage();

	//�I������
	void Finalize();

private://�����o�ϐ�
	//�E�B���h�D�n���h��
	HWND hwnd = nullptr;

	//�E�B���h�D�N���X�̐ݒ�
	WNDCLASSEX wndClassEx{};


public://�A�N�Z�b�T�u����
	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetHinstance() const { return wndClassEx.hInstance; }

	static WinApp* GetInstance();

private:
	WinApp()=default;
	~WinApp() = default;
public:
	WinApp(const WinApp& obj) = delete;
	WinApp& operator=(const WinApp& obj) = delete;
};
