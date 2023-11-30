#pragma once
#include <chrono>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <thread>
#include <vector>
#include <wrl.h>

#include "WinApp.h"

/*

*	DirectXCommon.h

*	DirectX基盤クラス

*/
class DirectXCommon final
{
public: // 省略
	//エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://シングルトンインスタンス
	static DirectXCommon* GetInstance();
public:
	
	//初期化(Windowsアプリケーション)
	void Initialize(const WinApp* winApp);
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
	ComPtr<ID3D12Device> device_ = nullptr;
	//DXGIファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> cmdAllocator_ = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	//スワップチェーンデスク
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	
	//レンダーターゲットビューデスク
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc_{};
	//レンダーターゲットビューヒープ
	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
	//深度ビューヒープ
	ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff_ = nullptr;
	// バックバッファ
	std::vector<ComPtr<ID3D12Resource>> backBuffers_;
	// フェンス
	ComPtr<ID3D12Fence> fence_ = nullptr;
	//フェンス値
	UINT64 fenceVal_ = 0;

	//Windowsアプリケーション
	const WinApp* winApp_ = nullptr;

	//リソースバリア
	D3D12_RESOURCE_BARRIER barrierDesc_{};

public://アクセッサ置き場
	//デバイス取得
	ID3D12Device* GetDevice() const { return device_.Get(); }
	//コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	//バックバッファ数を取得
	size_t GetBuckBufferCount()const { return backBuffers_.size(); }
private://メンバ関数
	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point referense_;
private:
	//コンストラクタ（シングルトンパターン）
	DirectXCommon() = default;
	//デストラクタ（シングルトンパターン）
	~DirectXCommon() = default;
public:
	//コピーコンストラクタの防止（シングルトンパターン）
	DirectXCommon(const DirectXCommon& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	DirectXCommon& operator=(const DirectXCommon& obj) = delete;
};
