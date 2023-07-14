#pragma once

#define DIRECTINPUT_VERSION		0x0800		//DirectInput�̃o�[�W�����w��

#include <dinput.h>
#include <DirectXMath.h>
#include "WinApp.h"
#include <Windows.h>
#include <wrl.h>

//����
class Input final
{
public:
	//�G�C���A�X�e���v���[�g��namespace�ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


private://�\���̗�
	typedef enum MouseButtonNum
	{
		Left,
		Right,
		CenterWheel,
	}MouseButton;

	typedef enum JSButtonNum
	{
		jsLeft,
		jsRight,
		jsUP,
		jsDOWN,
		jsA,
		jsB,
		jsX,
		jsY,
		jsLB,
		jsLT,
		jsRB,
		jsRT,
	}JSButton;

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
	//�R���g���[���[�f�o�C�X����
	BOOL CALLBACK EnumJoyStickProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
	void GenerateJoyStick();


	///<summary>
	///�L�[��������Ă��邩(�v���X)
	///<summary>
	/// <param name="keyNumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>������Ă��邩</returns>
	bool PushKey(BYTE keyNumber);
	///<summary>
	///�L�[����������Ă��邩(�g���K�[)
	///<summary>
	/// <param name="keyNumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>��������Ă��邩</returns>
	bool TriggerKey(BYTE keyNumber);
	///<summary>
	///�L�[�𗣂����u�Ԃ�(�����[�X)
	///<summary>
	/// <param name="keyNumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>��������</returns>
	bool ReleaseKey(BYTE keyNumber);
	

	///<summary>
	///�L�[��������Ă��邩(�v���X)
	///<summary>
	/// <param name="mouseNumber">�}�E�X�ԍ�(0 = ���@1 = �E�@2 = ����)</param>
	/// <returns>������Ă��邩</returns>
	bool PressMouse(int32_t mouseNumber);

	///<summary>
	///�L�[����������Ă��邩(�g���K�[)
	///<summary>
	/// <param name="mouseNumber">�}�E�X�ԍ�(0 = ���@1 = �E�@2 = ����)</param>
	/// <returns>��������Ă��邩</returns>
	bool TriggerMouse(int32_t mouseNumber);

	///<summary>
	///�L�[�𗣂����u�Ԃ�(�����[�X)
	///<summary>
	/// <param name="mouseNumber">�}�E�X�ԍ�(0 = ���@1 = �E�@2 = ����)</param>
	/// <returns>��������</returns>
	bool ReleaseMouse(int32_t mouseNumber);

	const DirectX::XMFLOAT2& GetMousePosition()const { return mousePos; }

	
private://�����o�ϐ�
	//DirectInput�̏�����
	ComPtr<IDirectInput8> directInput = nullptr;
	//�L�[�{�[�h
	ComPtr <IDirectInputDevice8> keyboard = nullptr;
	
	//�S�L�[�̏��
	BYTE keys[256] = {};
	BYTE preKeys[256] = {};

	//�}�E�X
	ComPtr <IDirectInputDevice8> mouse = nullptr;
	//�}�E�X�X�e�[�g
	DIMOUSESTATE2 mouseState;
	//1�t���[���O�̃}�E�X�X�e�[�g
	DIMOUSESTATE2 mouseStatePre;
	//�}�E�X���W
	DirectX::XMFLOAT2 mousePos;

	//�W���C�X�e�B�b�N(�R���g���[���[)
	ComPtr<IDirectInputDevice8> joyStick = nullptr;
	//�W���C�X�e�B�b�N�X�e�[�g
	DIJOYSTATE2 joyState;
	DIJOYSTATE2 joyStatePre;

private:
	Input() = default;
	~Input();
public:
	Input(const Input& obj) = delete;
	Input& operator=(const Input& obj) = delete;
};
