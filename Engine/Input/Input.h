#pragma once

#define DIRECTINPUT_VERSION		0x0800		//DirectInput�̃o�[�W�����w��

#include <dinput.h>
#include "WinApp.h"
#include <Windows.h>
#include <wrl.h>

//����
class Input
{
public:
	//�G�C���A�X�e���v���[�g��namespace�ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	//������
	void Initialize(WinApp* winApp);

	//�X�V
	void Update();

	///<summary>
	///�L�[��������Ă��邩(�v���X)
	///<summary>
	/// <param name="keyNumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>������Ă��邩</returns>
	bool PushKey(BYTE keyNumber);
	///<summary>
	///�L�[��������Ă��邩(�g���K�[)
	///<summary>
	/// <param name="keyNumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>������Ă��邩</returns>
	bool TriggerKey(BYTE keyNumber);
	
private://�����o�ϐ�
	//WindowsAPI
	WinApp* winApp_ = nullptr;
	//DirectInput�̏�����
	ComPtr<IDirectInput8> directInput = nullptr;
	//�L�[�{�[�h
	ComPtr <IDirectInputDevice8> keyboard = nullptr;

	//�S�L�[�̏��
	BYTE preKeys[256] = {};
	BYTE keys[256] = {};

};
