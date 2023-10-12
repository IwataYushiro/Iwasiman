#pragma once

#define DIRECTINPUT_VERSION		0x0800		//DirectInputのバージョン指定

#include <dinput.h>
#include <DirectXMath.h>
#include "WinApp.h"
#include <Windows.h>
#include <wrl.h>

/*

*	Input.h

*	キーボード、マウス入力

*/
class Input final
{
public:
	//エイリアステンプレートでnamespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


private://構造体類
	//マウスのボタン
	typedef enum MouseButtonNum
	{
		Left,			//左クリック
		Right,			//右クリック
		CenterWheel,	//ホイール
	}MouseButton;

	//ジョイスティックのボタン（コントローラー、ゲームパッド）
	typedef enum JSButtonNum
	{
		jsLeft,			//左
		jsRight,		//右
		jsUP,			//上
		jsDOWN,			//下
		jsA,			//A
		jsB,			//B
		jsX,			//X
		jsY,			//Y
		jsLB,			//Lボタン
		jsLT,			//Lトリガー
		jsRB,			//Rボタン
		jsRT,			//Rトリガー
	}JSButton;

	//ジョイスティックパラメータ
	struct PadParam
	{
		ComPtr<IDirectInputDevice8> joyStick;//ジョイスティック
		int find;//見つかったか
	};

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
	
	//ジョイスティック生成
	//void GenerateJoyStick();

	//キーが押されているか(プレス)(キー(DIK_SPACEとか))
	bool PushKey(BYTE keyNumber);
	//キーが押されているか(トリガー)(キー(DIK_SPACEとか))
	bool TriggerKey(BYTE keyNumber);
	//キーが押されているか(リリース)(キー(DIK_SPACEとか))
	bool ReleaseKey(BYTE keyNumber);
	
	
	//マウスが押されているか(プレス)(マウス番号(0 = 左　1 = 右　2 = 中央))
	bool PressMouse(int32_t mouseNumber);
	//マウスが押されているか(トリガー)(マウス番号(0 = 左　1 = 右　2 = 中央))
	bool TriggerMouse(int32_t mouseNumber);
	//マウスが押されているか(リリース)(マウス番号(0 = 左　1 = 右　2 = 中央))
	bool ReleaseMouse(int32_t mouseNumber);
	//マウス座標ゲット
	const DirectX::XMFLOAT2& GetMousePosition()const { return mousePos; }

	
private://メンバ変数
	//コントローラーデバイス生成
	//static BOOL CALLBACK EnumJoyStickProc(const DIDEVICEINSTANCE* lpddi, VOID* pvRef)noexcept;
	
	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	//キーボード
	ComPtr <IDirectInputDevice8> keyboard = nullptr;
	
	//全キーの状態
	BYTE keys[256] = {};
	//1フレーム前の全キーの状態
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
	//1フレーム前のジョイスティックステート
	DIJOYSTATE2 joyStatePre;
	//ジョイスティックかどうか
	bool isJoyStick = false;
private:
	//コンストラクタ（シングルトンパターン）
	Input() = default;
	//デストラクタ（シングルトンパターン）
	~Input();
public:
	//コピーコンストラクタの防止（シングルトンパターン）
	Input(const Input& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	Input& operator=(const Input& obj) = delete;

public://アクセッサ
	//DirectInputゲット
	IDirectInput8* GetDirectInput()const { return directInput.Get(); }
};
