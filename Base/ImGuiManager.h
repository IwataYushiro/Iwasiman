#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include <wrl.h>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

//ImGui管理
class ImGuiManager
{
private://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	
	//初期化
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	
	//更新
	void Update();
	//受付開始
	void Begin();
	//受付終了
	void End();
	
	void ImGuiStyleShowSample();

	void ImGuiMyFirstToolColor();
	
	//描画
	void Draw();
	
	//終了
	void Finalize();

private:
	//WinApp
	WinApp* winApp_ = nullptr;
	//DXCommon
	DirectXCommon* dxCommon_ = nullptr;
	//SRV用のデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;
};
