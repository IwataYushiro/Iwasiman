#pragma once

#define DIRECTINPUT_VERSION		0x0800		//DirectInputのバージョン指定

#include <dinput.h>
#include "WinApp.h"
#include <Windows.h>
#include <wrl.h>

//入力
class Input
{
public:
	//エイリアステンプレートでnamespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://シングルトンインスタンス
	static Input* GetInstance();
public:

	//初期化
	void Initialize(WinApp* winApp);

	//更新
	void Update();

	///<summary>
	///キーが押されているか(プレス)
	///<summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);
	///<summary>
	///キーが押されているか(トリガー)
	///<summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>押されているか</returns>
	bool TriggerKey(BYTE keyNumber);
	
private://メンバ変数
	//WindowsAPI
	WinApp* winApp_ = nullptr;
	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	//キーボード
	ComPtr <IDirectInputDevice8> keyboard = nullptr;

	//全キーの状態
	BYTE preKeys[256] = {};
	BYTE keys[256] = {};

};
