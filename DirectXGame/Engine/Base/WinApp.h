#pragma once

#pragma warning(push)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include <Windows.h>
#pragma warning(pop)

namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	WinApp.h

	*	Windowsアプリケーション

	*/
	class WinApp final
	{
	public://定数
		//ウィンドゥサイズ
		static const int WINDOW_WIDTH = 1280; //横幅
		static const int WINDOW_HEIGHT = 720; //縦幅

	public://メンバ関数

		//ウィンドゥプロシージャ(ウィンドゥハンドル、メッセージ、Windowsパラメータ、Longパラメータ)
		static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		//初期化
		void Initialize();

		//更新
		bool ProcessMessage();

		//終了処理
		void Finalize();

	private://メンバ変数
		//ウィンドゥハンドル
		HWND hwnd_ = nullptr;

		//ウィンドゥクラスの設定
		WNDCLASSEX wndClassEx_{};


	public://アクセッサ置き場
		//ウィンドゥハンドルゲット
		HWND GetHwnd() const { return hwnd_; }
		//ウィンドゥハンドルインスタンスゲット
		HINSTANCE GetHinstance() const { return wndClassEx_.hInstance; }
		//インスタンス取得（シングルトンパターン）
		static WinApp* GetInstance();

	private:
		//コンストラクタ（シングルトンパターン）
		WinApp() {};
		//デストラクタ（シングルトンパターン）
		~WinApp() = default;
	public:
		//コピーコンストラクタの防止（シングルトンパターン）
		WinApp(const WinApp& obj) = delete;
		// コピー代入演算子を禁止（シングルトンパターン）
		WinApp& operator=(const WinApp& obj) = delete;
	};
}