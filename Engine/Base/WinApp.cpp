#include "WinApp.h"
#include <imgui_impl_win32.h>

#pragma comment(lib,"winmm.lib")

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

	
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.lpfnWndProc = (WNDPROC)WindowProc;      //�E�B���h�D�v���V�[�W����ݒ� 
	wndClassEx.lpszClassName = L"DX12Sample";           //�E�B���h�D�N���X��
	wndClassEx.hInstance = GetModuleHandle(nullptr);   //�E�B���h�D�n���h��
	wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);  //�J�[�\���w��

	//�E�B���h�D�N���X��OS�ɓo�^
	RegisterClassEx(&wndClassEx);
	//�E�B���h�E�T�C�Y {X���W,Y���W,����,�c��}
	RECT wrc = { 0,0,window_width,window_height };
	//�����ŃT�C�Y��␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�D�쐬
	hwnd = CreateWindow(wndClassEx.lpszClassName,   //�N���X��
		L"DirectXGame IWASI Engine",				//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,					//�W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,							//�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,							//�\��Y���W(OS�ɔC����)
		wrc.right - wrc.left,					//�E�B���h�E����
		wrc.bottom - wrc.top,					//�E�B���h�E�c��
		nullptr,								//�e�E�B���h�E�n���h��
		nullptr,								//���j���[�n���h��
		wndClassEx.hInstance,							//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);								//�I�v�V����

	//�E�B���h�D��\����Ԃɂ���
	ShowWindow(hwnd, SW_SHOW);

	//�V�X�e���^�C�}�[�̕���\���グ��
	timeBeginPeriod(1);
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
	UnregisterClass(wndClassEx.lpszClassName, wndClassEx.hInstance);
}
