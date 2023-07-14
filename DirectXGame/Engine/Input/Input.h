#pragma once

#define DIRECTINPUT_VERSION		0x0800		//DirectInputのバージョン指定

#include <dinput.h>
#include <DirectXMath.h>
#include "WinApp.h"
#include <Windows.h>
#include <wrl.h>

//入力
class Input final
{
public:
	//エイリアステンプレートでnamespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


private://構造体類
	typedef enum MouseButtonNum
	{
		Left,
		Right,
		CenterWheel,
	}MouseButton;

	typedef enum JSButtonNum
	{
		jsLeft,
		jsRight,
		jsUP,
		jsDOWN,
		jsA,
		jsB,
		jsX,
		jsY,
		jsLB,
		jsLT,
		jsRB,
		jsRT,
	}JSButton;

public://シングルトンインスタンス
	static Input* GetInstance();
public:
	//初期化
	void Initialize();

	//更新
	void Update();

	//キーボードデバイス生成
	void GenerateKeyBoard();
	//マウスデバイス生成
	void GenerateMouse();
	//コントローラーデバイス生成
	BOOL CALLBACK EnumJoyStickProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
	void GenerateJoyStick();


	///<summary>
	///キーが押されているか(プレス)
	///<summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);
	///<summary>
	///キーが今押されているか(トリガー)
	///<summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>今押されているか</returns>
	bool TriggerKey(BYTE keyNumber);
	///<summary>
	///キーを離した瞬間か(リリース)
	///<summary>
	/// <param name="keyNumber">キー番号(DIK_0等)</param>
	/// <returns>離したか</returns>
	bool ReleaseKey(BYTE keyNumber);
	

	///<summary>
	///キーが押されているか(プレス)
	///<summary>
	/// <param name="mouseNumber">マウス番号(0 = 左　1 = 右　2 = 中央)</param>
	/// <returns>押されているか</returns>
	bool PressMouse(int32_t mouseNumber);

	///<summary>
	///キーが今押されているか(トリガー)
	///<summary>
	/// <param name="mouseNumber">マウス番号(0 = 左　1 = 右　2 = 中央)</param>
	/// <returns>今押されているか</returns>
	bool TriggerMouse(int32_t mouseNumber);

	///<summary>
	///キーを離した瞬間か(リリース)
	///<summary>
	/// <param name="mouseNumber">マウス番号(0 = 左　1 = 右　2 = 中央)</param>
	/// <returns>離したか</returns>
	bool ReleaseMouse(int32_t mouseNumber);

	const DirectX::XMFLOAT2& GetMousePosition()const { return mousePos; }

	
private://メンバ変数
	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	//キーボード
	ComPtr <IDirectInputDevice8> keyboard = nullptr;
	
	//全キーの状態
	BYTE keys[256] = {};
	BYTE preKeys[256] = {};

	//マウス
	ComPtr <IDirectInputDevice8> mouse = nullptr;
	//マウスステート
	DIMOUSESTATE2 mouseState;
	//1フレーム前のマウスステート
	DIMOUSESTATE2 mouseStatePre;
	//マウス座標
	DirectX::XMFLOAT2 mousePos;

	//ジョイスティック(コントローラー)
	ComPtr<IDirectInputDevice8> joyStick = nullptr;
	//ジョイスティックステート
	DIJOYSTATE2 joyState;
	DIJOYSTATE2 joyStatePre;

private:
	Input() = default;
	~Input();
public:
	Input(const Input& obj) = delete;
	Input& operator=(const Input& obj) = delete;
};
