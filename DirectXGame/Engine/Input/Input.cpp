#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace IwasiEngine;
/*

*	Input.cpp

*	キーボード、マウス入力

*/

Input* Input::GetInstance()
{
	//インスタンス取得
	static Input instance;
	return &instance;
}

Input::~Input()
{
	//入力情報の取得を終了
	if (keyboard_)keyboard_->Unacquire();
	if (mouse_)mouse_->Unacquire();
	if (isJoyStick_)
	{
		if (joyStick_)joyStick_->Unacquire();
	}
}

//初期化
void Input::Initialize()
{
	HRESULT result;

#pragma	region DirectInputの初期化

	result = DirectInput8Create(
		WinApp::GetInstance()->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));
#pragma endregion

	//キーボードデバイス
	GenerateKeyBoard();
	//マウスデバイス
	GenerateMouse();
	//コントローラーデバイス
	GenerateJoyStick();
}

//更新
void Input::Update()
{
	//前回のキー入力を保存
	memcpy(preKeys_, keys_, sizeof(keys_));
	//マウス
	mouseStatePre_ = mouseState_;
	//ジョイスティック
	joyStatePre_ = joyState_;

	//キーボード情報の取得開始
	keyboard_->Acquire();
	//全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(keys_), keys_);

	//マウス情報の取得開始
	mouse_->Acquire();
	//全マウスの入力状態を取得する
	mouse_->GetDeviceState(sizeof(mouseState_),&mouseState_);

	//マウスのスクリーン座標を取得
	POINT mouseScreenPos;
	GetCursorPos(&mouseScreenPos);

	//クライアントエリア座標に変換
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &mouseScreenPos);
	mousePos_.x = static_cast<float>(mouseScreenPos.x);
	mousePos_.y = static_cast<float>(mouseScreenPos.y);

	if (isJoyStick_)
	{
		joyStick_->Acquire();
		joyStick_->Poll();
		//全コントローラーの入力状態を取得する
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
	//キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::GenerateMouse()
{
	HRESULT result;
	//マウスデバイスの生成
	result = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = mouse_->SetDataFormat(&c_dfDIMouse2); //標準形式(拡張8ボタン)
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = mouse_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}



void Input::GenerateJoyStick()
{
	HRESULT result;
	
	//ジョイスティック列挙
	result = directInput_->EnumDevices(DI8DEVTYPE_GAMEPAD,
		EnumJoyStickProc,this, DIEDFL_ATTACHEDONLY);
	if (joyStick_) isJoyStick_ = true;

}


//キーが押されているか
bool Input::PushKey(const BYTE keyNumber) {

	//指定キーを押していればtrue
	if (keys_[keyNumber])
	{
		return true;
	}
	//そうでなければfalse
	return false;
}

//トリガー版
bool Input::TriggerKey(const BYTE keyNumber) {

	//指定キーを押していればtrue
	if (keys_[keyNumber] && !preKeys_[keyNumber])
	{
		return true;
	}
	//そうでなければfalse
	return false;
}

bool Input::ReleaseKey(const BYTE keyNumber)
{
	//1フレーム前の地点で指定キーを押していればtrue
	if (!keys_[keyNumber] && preKeys_[keyNumber])
	{
		return true;
	}
	//そうでなければfalse
	return false;
}

bool Input::PressMouse(const int32_t mouseNumber)
{
	//指定番号(0～2)が無いときはエラー
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState_.rgbButtons));
	if ((mouseState_.rgbButtons[mouseNumber]& mouseTrueNum_))
	{
		return true;
	}
	return false;
}

bool Input::TriggerMouse(const int32_t mouseNumber)
{
	//指定番号(0～2)が無いときはエラー
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
{//指定番号(0～2)が無いときはエラー
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
	//デバイス生成
	result = input->GetDirectInput()->CreateDevice(GUID_Joystick, &input->joyStick_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = input->joyStick_->SetDataFormat(&c_dfDIJoystick2); //標準形式(拡張8ボタン)
	assert(SUCCEEDED(result));

	//軸モード
	// 軸モードを絶対値モードとして設定
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;
	input->joyStick_->SetProperty(DIPROP_AXISMODE, &diprop.diph);

	//入力範囲のセット
	DIPROPRANGE diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMax = 1000;
	diprg.lMin = -1000;

	//X軸
	diprg.diph.dwObj = DIJOFS_X;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//Y軸
	diprg.diph.dwObj = DIJOFS_Y;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//Z軸
	diprg.diph.dwObj = DIJOFS_Z;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RX軸
	diprg.diph.dwObj = DIJOFS_RX;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RY軸
	diprg.diph.dwObj = DIJOFS_RY;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RZ軸
	diprg.diph.dwObj = DIJOFS_RZ;
	input->joyStick_->SetProperty(DIPROP_RANGE, &diprg.diph);

	//排他制御レベルのセット
	result = input->joyStick_->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));




	return DIENUM_STOP;
}
