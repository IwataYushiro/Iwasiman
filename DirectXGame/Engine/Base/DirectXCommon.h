#pragma once
#include <chrono>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <thread>
#include <vector>
#include <wrl.h>

#include "WinApp.h"

class DirectXCommon
{
public: // 省略
	//エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	
	//初期化
	void Initialize(WinApp* winApp);
	//デバイスの初期化
	void InitializeDevice();
	//コマンド関連の初期化
	void InitializeCommand();
	//スワップチェーンの初期化
	void InitializeSwapchain();
	//レンダーターゲットビューの初期化
	void InitializeRenderTargetView();
	//深度バッファの初期化
	void InitializeDepthBuffer();
	//深度ビュー作成
	void CreateDepthView();
	//フェンス初期化
	void InitializeFence();

	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();
private:
	//DirectX12デバイス
	ComPtr<ID3D12Device> device = nullptr;
	//DXGIファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	
	//レンダーターゲットビュー
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	//深度ビュー用
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;
	ComPtr<ID3D12Resource> depthBuff = nullptr;
	// バックバッファ
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	// フェンス
	ComPtr<ID3D12Fence> fence = nullptr;
	//フェンス値
	UINT64 fenceVal = 0;

	//WindowsAPI
	WinApp* winApp_ = nullptr;

	//リソースバリア
	D3D12_RESOURCE_BARRIER barrierDesc{};

public://アクセッサ置き場
	//デバイス取得
	ID3D12Device* GetDevice() const { return device.Get(); }
	//コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	//バックバッファ数を取得
	size_t GetBuckBufferCount()const { return backBuffers.size(); }
private://メンバ関数
	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point referense_;
};
