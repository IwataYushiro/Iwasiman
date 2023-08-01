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
	//入力情報の取得を終了
	if (keyboard)keyboard->Unacquire();
	if (mouse)mouse->Unacquire();
	if (isJoyStick)
	{
		if (joyStick)joyStick->Unacquire();
	}
}

//初期化
void Input::Initialize()
{
	HRESULT result;

#pragma	region DirectInputの初期化

	result = DirectInput8Create(
		WinApp::GetInstance()->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
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
	memcpy(preKeys, keys, sizeof(keys));
	//マウス
	mouseStatePre = mouseState;
	//ジョイスティック
	joyStatePre = joyState;

	//キーボード情報の取得開始
	keyboard->Acquire();
	//全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(keys), keys);

	//マウス情報の取得開始
	mouse->Acquire();
	//全マウスの入力状態を取得する
	mouse->GetDeviceState(sizeof(mouseState),&mouseState);

	//マウスのスクリーン座標を取得
	POINT mouseScreenPos;
	GetCursorPos(&mouseScreenPos);

	//クライアントエリア座標に変換
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &mouseScreenPos);
	mousePos.x = static_cast<float>(mouseScreenPos.x);
	mousePos.y = static_cast<float>(mouseScreenPos.y);

	if (isJoyStick)
	{
		joyStick->Acquire();
		joyStick->Poll();
		//全コントローラーの入力状態を取得する
		joyStick->GetDeviceState(sizeof(joyState), &joyState);
		if (joyState.rgdwPOV[0] != 0xFFFFFFFF)
		{
			OutputDebugStringA("ok");
		}
	}
}

void Input::GenerateKeyBoard()
{
	HRESULT result;
	//キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::GenerateMouse()
{
	HRESULT result;
	//マウスデバイスの生成
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = mouse->SetDataFormat(&c_dfDIMouse2); //標準形式(拡張8ボタン)
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = mouse->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}



void Input::GenerateJoyStick()
{
	HRESULT result;
	
	//ジョイスティック列挙
	result = directInput->EnumDevices(DI8DEVTYPE_GAMEPAD,
		EnumJoyStickProc,this, DIEDFL_ATTACHEDONLY);
	if (joyStick) isJoyStick = true;

}

//キーが押されているか
bool Input::PushKey(BYTE keyNumber) {

	//指定キーを押していればtrue
	if (keys[keyNumber])
	{
		return true;
	}
	//そうでなければfalse
	return false;
}

//トリガー版
bool Input::TriggerKey(BYTE keyNumber) {

	//指定キーを押していればtrue
	if (keys[keyNumber] && !preKeys[keyNumber])
	{
		return true;
	}
	//そうでなければfalse
	return false;
}

bool Input::ReleaseKey(BYTE keyNumber)
{
	//1フレーム前の地点で指定キーを押していればtrue
	if (!keys[keyNumber] && preKeys[keyNumber])
	{
		return true;
	}
	//そうでなければfalse
	return false;
}

bool Input::PressMouse(int32_t mouseNumber)
{
	//指定番号(0～2)が無いときはエラー
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState.rgbButtons));
	if ((mouseState.rgbButtons[mouseNumber]& 0x80))
	{
		return true;
	}
	return false;
}

bool Input::TriggerMouse(int32_t mouseNumber)
{
	//指定番号(0～2)が無いときはエラー
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
{//指定番号(0～2)が無いときはエラー
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseStatePre.rgbButtons));
	assert(0 <= mouseNumber && mouseNumber < _countof(mouseState.rgbButtons));
	if ((mouseStatePre.rgbButtons[mouseNumber] & 0x80)
		&& !(mouseState.rgbButtons[mouseNumber] & 0x80))
	{
		return true;
	}
	return false;
}

BOOL Input::EnumJoyStickProc(const DIDEVICEINSTANCE* lpddi, VOID* pvRef) noexcept
{
	HRESULT result;
	
	Input* input = (Input*)pvRef;
	//デバイス生成
	result = input->GetDirectInput()->CreateDevice(GUID_Joystick, &input->joyStick, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = input->joyStick->SetDataFormat(&c_dfDIJoystick2); //標準形式(拡張8ボタン)
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
	input->joyStick->SetProperty(DIPROP_AXISMODE, &diprop.diph);

	//入力範囲のセット
	DIPROPRANGE diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMax = 1000;
	diprg.lMin = -1000;

	//X軸
	diprg.diph.dwObj = DIJOFS_X;
	input->joyStick->SetProperty(DIPROP_RANGE, &diprg.diph);
	//Y軸
	diprg.diph.dwObj = DIJOFS_Y;
	input->joyStick->SetProperty(DIPROP_RANGE, &diprg.diph);
	//Z軸
	diprg.diph.dwObj = DIJOFS_Z;
	input->joyStick->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RX軸
	diprg.diph.dwObj = DIJOFS_RX;
	input->joyStick->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RY軸
	diprg.diph.dwObj = DIJOFS_RY;
	input->joyStick->SetProperty(DIPROP_RANGE, &diprg.diph);
	//RZ軸
	diprg.diph.dwObj = DIJOFS_RZ;
	input->joyStick->SetProperty(DIPROP_RANGE, &diprg.diph);

	//排他制御レベルのセット
	result = input->joyStick->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));




	return DIENUM_STOP;
}
