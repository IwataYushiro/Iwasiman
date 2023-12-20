#pragma once

#define DIRECTINPUT_VERSION		0x0800		//DirectInput�̃o�[�W�����w��

#include <dinput.h>
#include <DirectXMath.h>
#include "WinApp.h"
#include <Windows.h>
#include <wrl.h>

/*

*	Input.h

*	�L�[�{�[�h�A�}�E�X����

*/
namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	class Input final
	{
	public:
		//�G�C���A�X�e���v���[�g��namespace�ȗ�
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


	private://�\���̗�
		//�}�E�X�̃{�^��
		typedef enum MouseButtonNum
		{
			Left,			//���N���b�N
			Right,			//�E�N���b�N
			CenterWheel,	//�z�C�[��
		}MouseButton;

		//�W���C�X�e�B�b�N�̃{�^���i�R���g���[���[�A�Q�[���p�b�h�j
		typedef enum JSButtonNum
		{
			jsLeft,			//��
			jsRight,		//�E
			jsUP,			//��
			jsDOWN,			//��
			jsA,			//A
			jsB,			//B
			jsX,			//X
			jsY,			//Y
			jsLB,			//L�{�^��
			jsLT,			//L�g���K�[
			jsRB,			//R�{�^��
			jsRT,			//R�g���K�[
		}JSButton;

		//�W���C�X�e�B�b�N�p�����[�^
		struct PadParam
		{
			ComPtr<IDirectInputDevice8> joyStick;//�W���C�X�e�B�b�N
			int find;//����������
		};

	public://�V���O���g���C���X�^���X
		static Input* GetInstance();
	public:
		//������
		void Initialize();

		//�X�V
		void Update();

		//�L�[�{�[�h�f�o�C�X����
		void GenerateKeyBoard();
		//�}�E�X�f�o�C�X����
		void GenerateMouse();

		//�W���C�X�e�B�b�N����
		void GenerateJoyStick();

		//�L�[��������Ă��邩(�v���X)(�L�[(DIK_SPACE�Ƃ�))
		bool PushKey(const BYTE keyNumber);
		//�L�[��������Ă��邩(�g���K�[)(�L�[(DIK_SPACE�Ƃ�))
		bool TriggerKey(const BYTE keyNumber);
		//�L�[��������Ă��邩(�����[�X)(�L�[(DIK_SPACE�Ƃ�))
		bool ReleaseKey(const BYTE keyNumber);


		//�}�E�X��������Ă��邩(�v���X)(�}�E�X�ԍ�(0 = ���@1 = �E�@2 = ����))
		bool PressMouse(const int32_t mouseNumber);
		//�}�E�X��������Ă��邩(�g���K�[)(�}�E�X�ԍ�(0 = ���@1 = �E�@2 = ����))
		bool TriggerMouse(const int32_t mouseNumber);
		//�}�E�X��������Ă��邩(�����[�X)(�}�E�X�ԍ�(0 = ���@1 = �E�@2 = ����))
		bool ReleaseMouse(const int32_t mouseNumber);


	private://�����o�ϐ�
		//�R���g���[���[�f�o�C�X����
		static BOOL CALLBACK EnumJoyStickProc(const DIDEVICEINSTANCE* lpddi, VOID* pvRef)noexcept;

		//DirectInput�̏�����
		ComPtr<IDirectInput8> directInput_ = nullptr;
		//�L�[�{�[�h
		ComPtr <IDirectInputDevice8> keyboard_ = nullptr;
		//�L�[�̃o�b�t�@
		static const int32_t keysBufferNum_ = 256;
		//�S�L�[�̏��
		BYTE keys_[keysBufferNum_] = {};
		//1�t���[���O�̑S�L�[�̏��
		BYTE preKeys_[keysBufferNum_] = {};

		//�}�E�X
		ComPtr <IDirectInputDevice8> mouse_ = nullptr;
		//�}�E�X�X�e�[�g
		DIMOUSESTATE2 mouseState_;
		//1�t���[���O�̃}�E�X�X�e�[�g
		DIMOUSESTATE2 mouseStatePre_;
		//�}�E�X���W
		DirectX::XMFLOAT2 mousePos_;
		//�}�E�X��������Ă���Ƃ����ʂ��邽�߂ɕK�v�Ȍv�Z�ԍ�
		const BYTE mouseTrueNum_ = 0x80;

		//�W���C�X�e�B�b�N(�R���g���[���[)
		ComPtr<IDirectInputDevice8> joyStick_ = nullptr;
		//�W���C�X�e�B�b�N�X�e�[�g
		DIJOYSTATE2 joyState_;
		//1�t���[���O�̃W���C�X�e�B�b�N�X�e�[�g
		DIJOYSTATE2 joyStatePre_;
		//�W���C�X�e�B�b�N���ǂ���
		bool isJoyStick_ = false;
	private:
		//�R���X�g���N�^�i�V���O���g���p�^�[���j
		Input() = default;
		//�f�X�g���N�^�i�V���O���g���p�^�[���j
		~Input();
	public:
		//�R�s�[�R���X�g���N�^�̖h�~�i�V���O���g���p�^�[���j
		Input(const Input& obj) = delete;
		// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
		Input& operator=(const Input& obj) = delete;

	public://�A�N�Z�b�T
		//DirectInput�Q�b�g
		IDirectInput8* GetDirectInput()const { return directInput_.Get(); }
		//�}�E�X���W�Q�b�g
		const DirectX::XMFLOAT2& GetMousePosition()const { return mousePos_; }

	};
}