#pragma once

#pragma warning(push)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include <Windows.h>
#pragma warning(pop)

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	WinApp.h

	*	Windows�A�v���P�[�V����

	*/
	class WinApp final
	{
	public://�萔
		//�E�B���h�D�T�C�Y
		static const int WINDOW_WIDTH = 1280; //����
		static const int WINDOW_HEIGHT = 720; //�c��

	public://�����o�֐�

		//�E�B���h�D�v���V�[�W��(�E�B���h�D�n���h���A���b�Z�[�W�AWindows�p�����[�^�ALong�p�����[�^)
		static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		//������
		void Initialize();

		//�X�V
		bool ProcessMessage();

		//�I������
		void Finalize();

	private://�����o�ϐ�
		//�E�B���h�D�n���h��
		HWND hwnd_ = nullptr;

		//�E�B���h�D�N���X�̐ݒ�
		WNDCLASSEX wndClassEx_{};


	public://�A�N�Z�b�T�u����
		//�E�B���h�D�n���h���Q�b�g
		HWND GetHwnd() const { return hwnd_; }
		//�E�B���h�D�n���h���C���X�^���X�Q�b�g
		HINSTANCE GetHinstance() const { return wndClassEx_.hInstance; }
		//�C���X�^���X�擾�i�V���O���g���p�^�[���j
		static WinApp* GetInstance();

	private:
		//�R���X�g���N�^�i�V���O���g���p�^�[���j
		WinApp() {};
		//�f�X�g���N�^�i�V���O���g���p�^�[���j
		~WinApp() = default;
	public:
		//�R�s�[�R���X�g���N�^�̖h�~�i�V���O���g���p�^�[���j
		WinApp(const WinApp& obj) = delete;
		// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
		WinApp& operator=(const WinApp& obj) = delete;
	};
}