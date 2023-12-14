#include "WinApp.h"

#pragma warning(push)
#pragma warning(disable:4514)
#pragma warning(disable:4820)
#include <imgui_impl_win32.h>
#pragma warning(pop)

#pragma comment(lib,"winmm.lib")

using namespace IwasiEngine;
/*

*	WinApp.cpp

*	Windowsアプリケーション

*/

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler
(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//ウィンドゥプロシージャ
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウィンドゥが破棄された
	case WM_DESTROY:
		//OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//初期化
void WinApp::Initialize() {

	
	wndClassEx_.cbSize = sizeof(WNDCLASSEX);
	wndClassEx_.lpfnWndProc = (WNDPROC)WindowProc;      //ウィンドゥプロシージャを設定 
	wndClassEx_.lpszClassName = L"DX12Sample";           //ウィンドゥクラス名
	wndClassEx_.hInstance = GetModuleHandle(nullptr);   //ウィンドゥハンドル
	wndClassEx_.hCursor = LoadCursor(NULL, IDC_ARROW);  //カーソル指定
	
	//ウィンドゥクラスをOSに登録
	RegisterClassEx(&wndClassEx_);
	//ウィンドウサイズ {X座標,Y座標,横幅,縦幅}
	RECT wrc = { 0,0,WINDOW_WIDTH,WINDOW_HEIGHT };
	//自動でサイズを補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドゥ作成
	hwnd_ = CreateWindow(wndClassEx_.lpszClassName,   //クラス名
		L"Iwasiman",				//タイトルバーの文字
		//WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE,	//最大化だけ封印
		WS_OVERLAPPEDWINDOW,					//標準的なウィンドウスタイル WS_OVERLAPPED | WS_SYSMENU=閉じるだけ
		CW_USEDEFAULT,							//表示X座標(OSに任せる)
		CW_USEDEFAULT,							//表示Y座標(OSに任せる)
		wrc.right - wrc.left,					//ウィンドウ横幅
		wrc.bottom - wrc.top,					//ウィンドウ縦幅
		nullptr,								//親ウィンドウハンドル
		nullptr,								//メニューハンドル
		wndClassEx_.hInstance,							//呼び出しアプリケーションハンドル
		nullptr);								//オプション

	//ウィンドゥを表示状態にする
	ShowWindow(hwnd_, SW_SHOW);

	const UINT uPeriod = 1;//分解能を上げる用の変数
	//システムタイマーの分解能を上げる
	timeBeginPeriod(uPeriod);
}

//プロセスメッセージ
bool WinApp::ProcessMessage() {

	MSG msg{}; //メッセージ

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}

//終了処理
void WinApp::Finalize()
{
	//ウィンドゥクラスを登録解除
	UnregisterClass(wndClassEx_.lpszClassName, wndClassEx_.hInstance);
}

WinApp* WinApp::GetInstance()
{
	static WinApp instance;
	return &instance;
}
