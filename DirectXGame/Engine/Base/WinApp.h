#pragma once
#include <Windows.h>

class WinApp final
{
public://定数
	//ウィンドゥサイズ
	static const int window_width = 1280; //横幅
	static const int window_height = 720; //縦幅

public://メンバ関数
	
	//ウィンドゥプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//初期化
	void Initialize();

	//更新
	bool ProcessMessage();

	//終了処理
	void Finalize();

private://メンバ変数
	//ウィンドゥハンドル
	HWND hwnd = nullptr;

	//ウィンドゥクラスの設定
	WNDCLASSEX wndClassEx{};


public://アクセッサ置き場
	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetHinstance() const { return wndClassEx.hInstance; }

	static WinApp* GetInstance();

private:
	WinApp()=default;
	~WinApp() = default;
public:
	WinApp(const WinApp& obj) = delete;
	WinApp& operator=(const WinApp& obj) = delete;
};
