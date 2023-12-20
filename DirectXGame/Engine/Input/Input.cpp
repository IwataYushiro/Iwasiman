#include "Input.h"
#include <cassert>
#include <XYZ.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace IwasiEngine;

//静的メンバ変数の実体
Microsoft::WRL::ComPtr<IDirectInput8> Input::directInput_;

/*

*	Input.cpp

*	キーボード、マウス入力

*/

//ジョイスティックパラメータ
struct PadParam
{
	LPDIRECTINPUTDEVICE8* joyStickDevice;//ジョイスティック
	int32_t findCount;//見つかったか
};

//ゲームパッドプロパティ(軸モード)
bool SetupJoyStickProperty(LPDIRECTINPUTDEVICE8 didevice)
{
	// 軸モードを絶対値モードとして設定
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;	// 絶対値モードの指定(DIPROPAXISMODE_REL=相対値)
	if (FAILED(didevice->SetProperty(DIPROP_AXISMODE, &diprop.diph)))return false;

	//入力範囲のセット
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	const long MaxMin[XY_Num] = { 1000,-1000 };//XがMax,YがMin
	diprg.lMax = MaxMin[XY_X];
	diprg.lMin = MaxMin[XY_Y];

	//X軸
	diprg.diph.dwObj = DIJOFS_X;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//Y軸
	diprg.diph.dwObj = DIJOFS_Y;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//Z軸
	diprg.diph.dwObj = DIJOFS_Z;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//RX軸
	diprg.diph.dwObj = DIJOFS_RX;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//RY軸
	diprg.diph.dwObj = DIJOFS_RY;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;
	//RZ軸
	diprg.diph.dwObj = DIJOFS_RZ;
	if (FAILED(didevice->SetProperty(DIPROP_RANGE, &diprg.diph)))return false;

	return true;
}

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
	mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);

	//マウスのスクリーン座標を取得
	POINT mouseScreenPos;
	GetCursorPos(&mouseScreenPos);

	//クライアントエリア座標に変換
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &mouseScreenPos);
	mousePos_.x = static_cast<float>(mouseScreenPos.x);
	mousePos_.y = static_cast<float>(mouseScreenPos.y);

	//コントローラー情報の取得
	if (!joyStick_)return;
	joyStick_->Acquire();
	joyStatePre_ = joyState_;
	//全コントローラーの入力状態を取得する
	joyStick_->GetDeviceState(sizeof(joyState_), &joyState_);

	if (joyState_.rgdwPOV[0] != 0xFFFFFFFF)
	{
		OutputDebugStringA("シャア！！！");
	}
	else if (joyState_.rgdwPOV[9000] != 0xFFFFFFFF)
	{
		OutputDebugStringA("アムロ！！！");
	}
	else if (joyState_.rgdwPOV[18000] != 0xFFFFFFFF)
	{
		OutputDebugStringA("情けない奴！！！");
	}
	else if (joyState_.rgdwPOV[27000] != 0xFFFFFFFF)
	{
		OutputDebugStringA("モニターが死ぬぅ！？");
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
	
	//ゲームパッド
	PadParam parameter{};

	parameter.findCount = 0;
	parameter.joyStickDevice = &joyStick_;

	//ゲームパッド列挙
	result = directInput_->EnumDevices(DI8DEVTYPE_GAMEPAD,
		EnumJoyStickProc, &parameter, DIEDFL_ATTACHEDONLY);
	//ジョイスティックも列挙
	result = directInput_->EnumDevices(DI8DEVTYPE_JOYSTICK,
		EnumJoyStickProc, &parameter, DIEDFL_ATTACHEDONLY);

	//どっちも見つからなかったら失敗
	if (parameter.findCount == 0) return;

	//ジョイスティック(コントローラー)制御開始
	StartJoyStickControl();

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
	if ((mouseState_.rgbButtons[mouseNumber] & mouseTrueNum_))
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

BOOL CALLBACK Input::EnumJoyStickProc(const LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	HRESULT result;
	//ローカル変数初期化
	PadParam* paramater = static_cast<PadParam*>(pvRef);	//パラメータ
	IDirectInputDevice8* didevice = nullptr;				//入力デバイス

	//発見したらコールバックを抜ける
	if (paramater->findCount > 0)return DIENUM_STOP;

	//デバイス生成
	result = directInput_->CreateDevice(lpddi->guidInstance,paramater->joyStickDevice, NULL);
	if(FAILED(result))return DIENUM_STOP;

	//入力データ形式のセット
	didevice = *paramater->joyStickDevice;
	result =didevice->SetDataFormat(&c_dfDIJoystick2); //標準形式(拡張8ボタン)
	if (FAILED(result))return DIENUM_STOP;

	//入力範囲のセット
	if (!SetupJoyStickProperty(didevice))return DIENUM_STOP;

	//排他制御レベルのセット
	result = didevice->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
	//発見数のカウント
	paramater->findCount++;

	return DIENUM_CONTINUE;
}

void Input::StartJoyStickControl()
{
	//デバイスが生成されてない
	if (!joyStick_)return;
	//ポーリング判定のための変数
	DIDEVCAPS cap;
	joyStick_->GetCapabilities(&cap);

	//ポーリング判定
	if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
	{
		//条件通りならポーリングを開始
		joyStick_->Acquire();
		joyStick_->Poll();
	}
}
