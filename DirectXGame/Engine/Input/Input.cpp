#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//初期化
void Input::Initialize(WinApp* winApp)
{
	HRESULT result;

	this->winApp_ = winApp;
#pragma	region DirectInputの初期化
	
	result = DirectInput8Create(
		winApp->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); //標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
#pragma endregion
}

//更新
void Input::Update()
{
	//前回のキー入力を保存
	memcpy(preKeys, keys, sizeof(keys));
	//キーボード情報の取得開始
	keyboard->Acquire();

	//全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(keys), keys);

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