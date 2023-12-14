#include "WinApp.h"

#pragma warning(push)
#pragma warning(disable:4514)
#pragma warning(disable:4820)
#include <imgui_impl_win32.h>
#pragma warning(pop)

#pragma comment(lib,"winmm.lib")

using namespace IwasiEngine;
/*

*	WinApp.cpp

*	Windows�A�v���P�[�V����

*/

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler
(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//�E�B���h�D�v���V�[�W��
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg)
	{
		//�E�B���h�D���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��ăA�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}
	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//������
void WinApp::Initialize() {

	
	wndClassEx_.cbSize = sizeof(WNDCLASSEX);
	wndClassEx_.lpfnWndProc = (WNDPROC)WindowProc;      //�E�B���h�D�v���V�[�W����ݒ� 
	wndClassEx_.lpszClassName = L"DX12Sample";           //�E�B���h�D�N���X��
	wndClassEx_.hInstance = GetModuleHandle(nullptr);   //�E�B���h�D�n���h��
	wndClassEx_.hCursor = LoadCursor(NULL, IDC_ARROW);  //�J�[�\���w��
	
	//�E�B���h�D�N���X��OS�ɓo�^
	RegisterClassEx(&wndClassEx_);
	//�E�B���h�E�T�C�Y {X���W,Y���W,����,�c��}
	RECT wrc = { 0,0,WINDOW_WIDTH,WINDOW_HEIGHT };
	//�����ŃT�C�Y��␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�D�쐬
	hwnd_ = CreateWindow(wndClassEx_.lpszClassName,   //�N���X��
		L"Iwasiman",				//�^�C�g���o�[�̕���
		//WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE,	//�ő剻��������
		WS_OVERLAPPEDWINDOW,					//�W���I�ȃE�B���h�E�X�^�C�� WS_OVERLAPPED | WS_SYSMENU=���邾��
		CW_USEDEFAULT,							//�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,							//�\��Y���W(OS�ɔC����)
		wrc.right - wrc.left,					//�E�B���h�E����
		wrc.bottom - wrc.top,					//�E�B���h�E�c��
		nullptr,								//�e�E�B���h�E�n���h��
		nullptr,								//���j���[�n���h��
		wndClassEx_.hInstance,							//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);								//�I�v�V����

	//�E�B���h�D��\����Ԃɂ���
	ShowWindow(hwnd_, SW_SHOW);

	const UINT uPeriod = 1;//����\���グ��p�̕ϐ�
	//�V�X�e���^�C�}�[�̕���\���グ��
	timeBeginPeriod(uPeriod);
}

//�v���Z�X���b�Z�[�W
bool WinApp::ProcessMessage() {

	MSG msg{}; //���b�Z�[�W

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}

//�I������
void WinApp::Finalize()
{
	//�E�B���h�D�N���X��o�^����
	UnregisterClass(wndClassEx_.lpszClassName, wndClassEx_.hInstance);
}

WinApp* WinApp::GetInstance()
{
	static WinApp instance;
	return &instance;
}
