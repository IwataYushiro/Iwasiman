#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

Input::~Input()
{
	//���͏��̎擾���I��
	if (keyboard)
	{
		keyboard->Unacquire();
	}
	if (mouse)
	{
		mouse->Unacquire();
	}
}

//������
void Input::Initialize()
{
	HRESULT result;

#pragma	region DirectInput�̏�����

	result = DirectInput8Create(
		WinApp::GetInstance()->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
#pragma endregion

	//�L�[�{�[�h�f�o�C�X
	GenerateKeyBoard();
	//�}�E�X�f�o�C�X
	GenerateMouse();
}

//�X�V
void Input::Update()
{
	//�O��̃L�[���͂�ۑ�
	memcpy(preKeys, keys, sizeof(keys));
	//�}�E�X
	mouseStatePre = mouseState;

	//�L�[�{�[�h���̎擾�J�n
	keyboard->Acquire();
	//�S�L�[�̓��͏�Ԃ��擾����
	keyboard->GetDeviceState(sizeof(keys), keys);

	//�}�E�X���̎擾�J�n
	mouse->Acquire();
	//�S�}�E�X�̓��͏�Ԃ��擾����
	mouse->GetDeviceState(sizeof(mouseState),&mouseState);

	//�}�E�X�̃X�N���[�����W���擾
	POINT mouseScreenPos;
	GetCursorPos(&mouseScreenPos);

	//�N���C�A���g�G���A���W�ɕϊ�
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &mouseScreenPos);
	mousePos.x = static_cast<float>(mouseScreenPos.x);
	mousePos.y = static_cast<float>(mouseScreenPos.y);
}

void Input::GenerateKeyBoard()
{
	HRESULT result;
	//�L�[�{�[�h�f�o�C�X�̐���
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); //�W���`��
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::GenerateMouse()
{
	HRESULT result;
	//�}�E�X�f�o�C�X�̐���
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = mouse->SetDataFormat(&c_dfDIMouse2); //�W���`��(�g��8�{�^��)
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = mouse->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
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

bool Input::ReleaseKey(BYTE keyNumber)
{
	//1�t���[���O�̒n�_�Ŏw��L�[�������Ă����true
	if (!keys[keyNumber] && preKeys[keyNumber])
	{
		return true;
	}
	//�����łȂ����false
	return false;
}

bool Input::PressMouse(int32_t mouseNumber)
{
	//�w��ԍ�(0�`2)�������Ƃ��̓G���[
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState.rgbButtons));
	if ((mouseState.rgbButtons[mouseNumber]& 0x80))
	{
		return true;
	}
	return false;
}

bool Input::TriggerMouse(int32_t mouseNumber)
{
	//�w��ԍ�(0�`2)�������Ƃ��̓G���[
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseStatePre.rgbButtons));
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState.rgbButtons));
	if (!(mouseStatePre.rgbButtons[mouseNumber] & 0x80)
		&& (mouseState.rgbButtons[mouseNumber] & 0x80))
	{
		return true;
	}
	return false;
}

bool Input::ReleaseMouse(int32_t mouseNumber)
{//�w��ԍ�(0�`2)�������Ƃ��̓G���[
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseStatePre.rgbButtons));
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState.rgbButtons));
	if ((mouseStatePre.rgbButtons[mouseNumber] & 0x80)
		&& !(mouseState.rgbButtons[mouseNumber] & 0x80))
	{
		return true;
	}
	return false;
}
