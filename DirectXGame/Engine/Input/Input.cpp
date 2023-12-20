#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace IwasiEngine;
/*

*	Input.cpp

*	�L�[�{�[�h�A�}�E�X����

*/

Input* Input::GetInstance()
{
	//�C���X�^���X�擾
	static Input instance;
	return &instance;
}

Input::~Input()
{
	//���͏��̎擾���I��
	if (keyboard_)keyboard_->Unacquire();
	if (mouse_)mouse_->Unacquire();
	if (isJoyStick_)
	{
		if (joyStick_)joyStick_->Unacquire();
	}
}

//������
void Input::Initialize()
{
	HRESULT result;

#pragma	region DirectInput�̏�����

	result = DirectInput8Create(
		WinApp::GetInstance()->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));
#pragma endregion

	//�L�[�{�[�h�f�o�C�X
	GenerateKeyBoard();
	//�}�E�X�f�o�C�X
	GenerateMouse();
	//�R���g���[���[�f�o�C�X
	GenerateJoyStick();
}

//�X�V
void Input::Update()
{
	//�O��̃L�[���͂�ۑ�
	memcpy(preKeys_, keys_, sizeof(keys_));
	//�}�E�X
	mouseStatePre_ = mouseState_;
	//�W���C�X�e�B�b�N
	joyStatePre_ = joyState_;

	//�L�[�{�[�h���̎擾�J�n
	keyboard_->Acquire();
	//�S�L�[�̓��͏�Ԃ��擾����
	keyboard_->GetDeviceState(sizeof(keys_), keys_);

	//�}�E�X���̎擾�J�n
	mouse_->Acquire();
	//�S�}�E�X�̓��͏�Ԃ��擾����
	mouse_->GetDeviceState(sizeof(mouseState_),&mouseState_);

	//�}�E�X�̃X�N���[�����W���擾
	POINT mouseScreenPos;
	GetCursorPos(&mouseScreenPos);

	//�N���C�A���g�G���A���W�ɕϊ�
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &mouseScreenPos);
	mousePos_.x = static_cast<float>(mouseScreenPos.x);
	mousePos_.y = static_cast<float>(mouseScreenPos.y);

	if (isJoyStick_)
	{
		joyStick_->Acquire();
		joyStick_->Poll();
		//�S�R���g���[���[�̓��͏�Ԃ��擾����
		joyStick_->GetDeviceState(sizeof(joyState_), &joyState_);
		if (joyState_.rgdwPOV[0] != 0xFFFFFFFF)
		{
			OutputDebugStringA("ok");
		}
	}
}

void Input::GenerateKeyBoard()
{
	HRESULT result;
	//�L�[�{�[�h�f�o�C�X�̐���
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard); //�W���`��
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::GenerateMouse()
{
	HRESULT result;
	//�}�E�X�f�o�C�X�̐���
	result = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = mouse_->SetDataFormat(&c_dfDIMouse2); //�W���`��(�g��8�{�^��)
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = mouse_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}



void Input::GenerateJoyStick()
{
	HRESULT result;
	
	//�W���C�X�e�B�b�N��
	result = directInput_->EnumDevices(DI8DEVTYPE_GAMEPAD,
		EnumJoyStickProc,this, DIEDFL_ATTACHEDONLY);
	if (joyStick_) isJoyStick_ = true;

}


//�L�[��������Ă��邩
bool Input::PushKey(const BYTE keyNumber) {

	//�w��L�[�������Ă����true
	if (keys_[keyNumber])
	{
		return true;
	}
	//�����łȂ����false
	return false;
}

//�g���K�[��
bool Input::TriggerKey(const BYTE keyNumber) {

	//�w��L�[�������Ă����true
	if (keys_[keyNumber] && !preKeys_[keyNumber])
	{
		return true;
	}
	//�����łȂ����false
	return false;
}

bool Input::ReleaseKey(const BYTE keyNumber)
{
	//1�t���[���O�̒n�_�Ŏw��L�[�������Ă����true
	if (!keys_[keyNumber] && preKeys_[keyNumber])
	{
		return true;
	}
	//�����łȂ����false
	return false;
}

bool Input::PressMouse(const int32_t mouseNumber)
{
	//�w��ԍ�(0�`2)�������Ƃ��̓G���[
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState_.rgbButtons));
	if ((mouseState_.rgbButtons[mouseNumber]& mouseTrueNum_))
	{
		return true;
	}
	return false;
}

bool Input::TriggerMouse(const int32_t mouseNumber)
{
	//�w��ԍ�(0�`2)�������Ƃ��̓G���[
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseStatePre_.rgbButtons));
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState_.rgbButtons));
	if (!(mouseStatePre_.rgbButtons[mouseNumber] & mouseTrueNum_)
		&& (mouseState_.rgbButtons[mouseNumber] & mouseTrueNum_))
	{
		return true;
	}
	return false;
}

bool Input::ReleaseMouse(const int32_t mouseNumber)
{//�w��ԍ�(0�`2)�������Ƃ��̓G���[
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseStatePre_.rgbButtons));
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState_.rgbButtons));
	if ((mouseStatePre_.rgbButtons[mouseNumber] & mouseTrueNum_)
		&& !(mouseState_.rgbButtons[mouseNumber] & mouseTrueNum_))
	{
		return true;
	}
	return false;
}

BOOL Input::EnumJoyStickProc([[maybe_unused]]const DIDEVICEINSTANCE* lpddi, VOID* pvRef) noexcept
{
	HRESULT result;
	
	Input* input = (Input*)pvRef;
	//�f�o�C�X����
	result = input->GetDirectInput()->CreateDevice(GUID_Joystick, &input->joyStick_, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = input->joyStick_->SetDataFormat(&c_dfDIJoystick2); //�W���`��(�g��8�{�^��)
	assert(SUCCEEDED(result));

	//�����[�h
	// �����[�h���Βl���[�h�Ƃ��Đݒ�
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;
	input->joyStick_->SetProperty(DIPROP_AXISMODE, &diprop.diph);

	//���͔͈͂̃Z�b�g
	DIPROPRANGE diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMax = 1000;
	diprg.lMin = -1000;

	//X��
	diprg.diph.dwObj = DIJOFS_X;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//Y��
	diprg.diph.dwObj = DIJOFS_Y;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//Z��
	diprg.diph.dwObj = DIJOFS_Z;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RX��
	diprg.diph.dwObj = DIJOFS_RX;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RY��
	diprg.diph.dwObj = DIJOFS_RY;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RZ��
	diprg.diph.dwObj = DIJOFS_RZ;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);

	//�r�����䃌�x���̃Z�b�g
	result = input->joyStick_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));




	return DIENUM_STOP;
}
