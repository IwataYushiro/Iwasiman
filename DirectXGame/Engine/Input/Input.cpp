#include "Input.h"
#include <cassert>
#include <XYZ.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace IwasiEngine;

//�ÓI�����o�ϐ��̎���
Microsoft::WRL::ComPtr<IDirectInput8> Input::directInput_;

/*

*	Input.cpp

*	�L�[�{�[�h�A�}�E�X����

*/

//�W���C�X�e�B�b�N�p�����[�^
struct PadParam
{
	LPDIRECTINPUTDEVICE8* joyStickDevice;//�W���C�X�e�B�b�N
	int32_t findCount;//����������
};

//�Q�[���p�b�h�v���p�e�B(�����[�h)
bool SetupJoyStickProperty(LPDIRECTINPUTDEVICE8 didevice)
{
	// �����[�h���Βl���[�h�Ƃ��Đݒ�
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;	// ��Βl���[�h�̎w��(DIPROPAXISMODE_REL=���Βl)
	if (FAILED(didevice->SetProperty(DIPROP_AXISMODE, &diprop.diph)))return false;

	//���͔͈͂̃Z�b�g
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	const long MaxMin[XY_Num] = { 1000,-1000 };//X��Max,Y��Min
	diprg.lMax = MaxMin[XY_X];
	diprg.lMin = MaxMin[XY_Y];

	//X��
	diprg.diph.dwObj = DIJOFS_X;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//Y��
	diprg.diph.dwObj = DIJOFS_Y;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//Z��
	diprg.diph.dwObj = DIJOFS_Z;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//RX��
	diprg.diph.dwObj = DIJOFS_RX;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//RY��
	diprg.diph.dwObj = DIJOFS_RY;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//RZ��
	diprg.diph.dwObj = DIJOFS_RZ;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;

	return true;
}

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
	mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);

	//�}�E�X�̃X�N���[�����W���擾
	POINT mouseScreenPos;
	GetCursorPos(&mouseScreenPos);

	//�N���C�A���g�G���A���W�ɕϊ�
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &mouseScreenPos);
	mousePos_.x = static_cast<float>(mouseScreenPos.x);
	mousePos_.y = static_cast<float>(mouseScreenPos.y);

	//�R���g���[���[���̎擾
	if (!joyStick_)return;
	joyStick_->Acquire();
	joyStatePre_ = joyState_;
	//�S�R���g���[���[�̓��͏�Ԃ��擾����
	joyStick_->GetDeviceState(sizeof(joyState_), &joyState_);

	if (joyState_.rgdwPOV[0] != 0xFFFFFFFF)
	{
		OutputDebugStringA("�V���A�I�I�I");
	}
	else if (joyState_.rgdwPOV[9000] != 0xFFFFFFFF)
	{
		OutputDebugStringA("�A�����I�I�I");
	}
	else if (joyState_.rgdwPOV[18000] != 0xFFFFFFFF)
	{
		OutputDebugStringA("��Ȃ��z�I�I�I");
	}
	else if (joyState_.rgdwPOV[27000] != 0xFFFFFFFF)
	{
		OutputDebugStringA("���j�^�[�����ʂ��I�H");
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
	
	//�Q�[���p�b�h
	PadParam parameter{};

	parameter.findCount = 0;
	parameter.joyStickDevice = &joyStick_;

	//�Q�[���p�b�h��
	result = directInput_->EnumDevices(DI8DEVTYPE_GAMEPAD,
		EnumJoyStickProc, &parameter, DIEDFL_ATTACHEDONLY);
	//�W���C�X�e�B�b�N����
	result = directInput_->EnumDevices(DI8DEVTYPE_JOYSTICK,
		EnumJoyStickProc, &parameter, DIEDFL_ATTACHEDONLY);

	//�ǂ�����������Ȃ������玸�s
	if (parameter.findCount == 0) return;

	//�W���C�X�e�B�b�N(�R���g���[���[)����J�n
	StartJoyStickControl();

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
	if ((mouseState_.rgbButtons[mouseNumber] & mouseTrueNum_))
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

BOOL CALLBACK Input::EnumJoyStickProc(const LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	HRESULT result;
	//���[�J���ϐ�������
	PadParam* paramater = static_cast<PadParam*>(pvRef);	//�p�����[�^
	IDirectInputDevice8* didevice = nullptr;				//���̓f�o�C�X

	//����������R�[���o�b�N�𔲂���
	if (paramater->findCount > 0)return DIENUM_STOP;

	//�f�o�C�X����
	result = directInput_->CreateDevice(lpddi->guidInstance,paramater->joyStickDevice, NULL);
	if(FAILED(result))return DIENUM_STOP;

	//���̓f�[�^�`���̃Z�b�g
	didevice = *paramater->joyStickDevice;
	result =didevice->SetDataFormat(&c_dfDIJoystick2); //�W���`��(�g��8�{�^��)
	if (FAILED(result))return DIENUM_STOP;

	//���͔͈͂̃Z�b�g
	if (!SetupJoyStickProperty(didevice))return DIENUM_STOP;

	//�r�����䃌�x���̃Z�b�g
	result = didevice->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
	//�������̃J�E���g
	paramater->findCount++;

	return DIENUM_CONTINUE;
}

void Input::StartJoyStickControl()
{
	//�f�o�C�X����������ĂȂ�
	if (!joyStick_)return;
	//�|�[�����O����̂��߂̕ϐ�
	DIDEVCAPS cap;
	joyStick_->GetCapabilities(&cap);

	//�|�[�����O����
	if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
	{
		//�����ʂ�Ȃ�|�[�����O���J�n
		joyStick_->Acquire();
		joyStick_->Poll();
	}
}
