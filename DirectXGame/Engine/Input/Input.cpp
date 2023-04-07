#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//������
void Input::Initialize(WinApp* winApp)
{
	HRESULT result;

	this->winApp_ = winApp;
#pragma	region DirectInput�̏�����
	
	result = DirectInput8Create(
		winApp->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//�L�[�{�[�h�f�o�C�X�̐���
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); //�W���`��
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
#pragma endregion
}

//�X�V
void Input::Update()
{
	//�O��̃L�[���͂�ۑ�
	memcpy(preKeys, keys, sizeof(keys));
	//�L�[�{�[�h���̎擾�J�n
	keyboard->Acquire();

	//�S�L�[�̓��͏�Ԃ��擾����
	keyboard->GetDeviceState(sizeof(keys), keys);

}

//�L�[��������Ă��邩
bool Input::PushKey(BYTE keyNumber) {

	//�w��L�[�������Ă����true
	if (keys[keyNumber])
	{
		return true;
	}
	//�����łȂ����false
	return false;
}

//�g���K�[��
bool Input::TriggerKey(BYTE keyNumber) {

	//�w��L�[�������Ă����true
	if (keys[keyNumber] && !preKeys[keyNumber])
	{
		return true;
	}
	//�����łȂ����false
	return false;
}