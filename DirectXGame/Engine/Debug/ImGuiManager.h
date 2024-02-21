#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include <wrl.h>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	ImGuiManager.h

	*	ImGui管理

	*/
	class ImGuiManager final
	{
	private://エイリアス
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public://シングルトンインスタンス
		static ImGuiManager* GetInstance();

	public://メンバ関数

		//初期化(Windowsアプリケーション、DirectX基盤)
		void Initialize();

		//更新
		void Update();
		//受付開始
		void Begin();
		//受付終了
		void End();

		//ImGuiのサンプル関数（スタイルチェンジ）
		void ImGuiStyleShowSample();

		//ImGuiのサンプル関数（MyFirstTool）
		void ImGuiMyFirstToolColor();

		//描画
		void Draw();

		//終了
		void Finalize();

	private:
		//WinApp
		const WinApp* winApp_ = nullptr;
		//DXCommon
		const DirectXCommon* dxCommon_ = nullptr;
		//SRV用のデスクリプタヒープ
		ComPtr<ID3D12DescriptorHeap> srvHeap_;
	private:
		//コンストラクタ（シングルトンパターン）
		ImGuiManager() = default;
		//デストラクタ（シングルトンパターン）
		~ImGuiManager() = default;
	public:
		//コピーコンストラクタの防止（シングルトンパターン）
		ImGuiManager(const ImGuiManager& obj) = delete;
		// コピー代入演算子を禁止（シングルトンパターン）
		ImGuiManager& operator=(const ImGuiManager& obj) = delete;
	};
}