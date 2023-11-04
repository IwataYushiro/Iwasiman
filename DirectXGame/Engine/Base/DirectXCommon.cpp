#include <DirectXCommon.h>

#include <cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;

/*

*	DirectXCommon.cpp

*	DirectX基盤クラス

*/

DirectXCommon* DirectXCommon::GetInstance()
{
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize(WinApp* winApp)
{
	//NULLチェック
	assert(winApp);

	this->winApp_ = winApp;

#pragma region DirectX初期化
	//FPS固定初期化
	InitializeFixFPS();
	
	//デバイスの初期化
	InitializeDevice();
	//コマンド関連の初期化
	InitializeCommand();
	//スワップチェーンの初期化
	InitializeSwapchain();
	//レンダーターゲットビューの初期化
	InitializeRenderTargetView();
	//深度バッファの初期化
	InitializeDepthBuffer();
	//フェンス初期化
	InitializeFence();
#pragma endregion
}

void DirectXCommon::InitializeDevice()
{
#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	HRESULT result;

#pragma region デバッグレイヤ

#pragma endregion
#pragma region アダプタの列挙
	//DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//アダプター列挙用
	std::vector<ComPtr<IDXGIAdapter4>> adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//パフォーマンスが高いものから順に、全てのアダプターを列挙
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		//動的配列に追加
		adapters.push_back(tmpAdapter);
	}
#pragma endregion

#pragma region アダプタの選別
	//妥当なアダプタを選別
	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプタの情報を取得
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[i].Get();
			break;
		}
	}
#pragma endregion

#pragma region デバイスの生成
	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result == S_OK) {
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion
}

void DirectXCommon::InitializeCommand()
{
	HRESULT result;

#pragma region コマンドリストの生成
	// コマンドアロケータを生成
	result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator_));
	assert(SUCCEEDED(result));

	// コマンドリストを生成
	result = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region コマンドキューの生成
	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));
#pragma endregion
}

void DirectXCommon::InitializeSwapchain()
{
	HRESULT result;

#pragma region スワップチェーンの生成
	//マジックナンバー撲滅用のプリセット
	struct SwapChainPreset
	{
		const UINT width = 1280;
		const UINT height = 720;
		const UINT sampleDescCount = 1;
		const UINT bufferCount = 2;
	};
	SwapChainPreset preset;

	// スワップチェーンの設定
	swapChainDesc_.Width = preset.width;
	swapChainDesc_.Height = preset.height;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
	swapChainDesc_.SampleDesc.Count = preset.sampleDescCount; // マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapChainDesc_.BufferCount = preset.bufferCount; // バッファ数を2つに設定
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// スワップチェーンの生成
	//IDXGISwapChain1のComPtrを用意
	ComPtr<IDXGISwapChain1> swapChain1;

	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winApp_->GetHwnd(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1);

	//生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換
	swapChain1.As(&swapChain_);

	assert(SUCCEEDED(result));
#pragma endregion
}

void DirectXCommon::InitializeRenderTargetView()
{

#pragma region レンダビューターゲット
	// デスクリプタヒープの設定

	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount; // 裏表の2つ
	// デスクリプタヒープの生成
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));

	// バックバッファ
	backBuffers_.resize(swapChainDesc_.BufferCount);

	// スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers_.size(); i++) {
		// スワップチェーンからバッファを取得
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));
		// デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		// 裏か表かでアドレスがずれる
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		// レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		// シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}

#pragma endregion
}

void DirectXCommon::InitializeDepthBuffer()
{

	HRESULT result;
	//プリセット
	const UINT16 depthOrArraySize = 1;
	const UINT sampleDescCount = 1;
	//深度バッファ設定
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = winApp_->WINDOW_WIDTH;		//レンダーターゲットに合わせる
	depthResourceDesc.Height = winApp_->WINDOW_HEIGHT;	//レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = depthOrArraySize;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	depthResourceDesc.SampleDesc.Count = sampleDescCount;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	const float defaultDepthNum = 1.0f;
	depthClearValue.DepthStencil.Depth = defaultDepthNum;//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//深度バッファ生成

	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値書き込みに必要
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));

	//深度ビュー用デスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	const UINT descriptorNum = 1;
	dsvHeapDesc.NumDescriptors = descriptorNum;//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; //デプスステンシルビュー

	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
	CreateDepthView();

}
void DirectXCommon::CreateDepthView()
{
	//深度ビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; //深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,
		dsvHeap_->GetCPUDescriptorHandleForHeapStart());

}
void DirectXCommon::InitializeFence()
{
	HRESULT result;
#pragma region フェンス生成

	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
#pragma endregion
}

void DirectXCommon::PreDraw()
{
	// バックバッファの番号を取得(2つなので0番か1番)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	// 1.リソースバリアで書き込み可能に変更

	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get(); // バックバッファを指定
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
	const UINT barrierNum = 1;
	commandList_->ResourceBarrier(barrierNum, &barrierDesc_);

	// 2.描画先の変更
	const int RTVNum = 1;//レンダーターゲットビューの数
	// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += static_cast<unsigned long long>(bbIndex) * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 3.画面クリア			R	  G		B	A
	const FLOAT clearColor[] = { 0.25f,0.25f,0.25f,0.0f }; //青っぽい色
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	//深度バッファクリア
	const float depthNum = 1.0f;
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, depthNum, 0, 0, nullptr);

	// 4.描画コマンドここから
	//ビューポート設定コマンド
	const float maxDepthNum = 1.0f;

	D3D12_VIEWPORT viewport{};
	viewport.Width = WinApp::WINDOW_WIDTH;
	viewport.Height = WinApp::WINDOW_HEIGHT;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = maxDepthNum;
	//ビューポート設定コマンドを、コマンドリストに積む
	commandList_->RSSetViewports(RTVNum, &viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + WinApp::WINDOW_WIDTH;
	scissorRect.top = 0;
	scissorRect.bottom = scissorRect.top + WinApp::WINDOW_HEIGHT;
	//シザー矩形設定コマンドを、コマンドリストに積む
	commandList_->RSSetScissorRects(RTVNum, &scissorRect);
}

void DirectXCommon::PostDraw()
{
	HRESULT result;

	// 5.リソースバリアを戻す
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//描画状態から
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			//表示状態へ
	commandList_->ResourceBarrier(1, &barrierDesc_);

	// コマンドのフラッシュ

	// 命令のクローズ
	result = commandList_->Close();
	assert(SUCCEEDED(result));
	// コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// 画面に表示するバッファをフリップ(裏表の入替え)
	const UINT syncInterval = 1;
	result = swapChain_->Present(syncInterval, 0);
	assert(SUCCEEDED(result));
	//FPS固定
	UpdateFixFPS();

	//コマンドの実行完了を待つ
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//キューをクリア
	result = cmdAllocator_->Reset();
	assert(SUCCEEDED(result));
	//再びコマンドリストを貯める準備
	result = commandList_->Reset(cmdAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}

void DirectXCommon::InitializeFixFPS()
{
	referense_ = std::chrono::steady_clock::now();
}

void DirectXCommon::UpdateFixFPS()
{
	//1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	//1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinChackTime(uint64_t(1000000.0f / 65.0f));

	//現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - referense_);

	//1/60秒(よりわずかに短い期間)経っていない場合
	if (elapsed < kMinChackTime)
	{
		//1/60秒経過するまで微小なsleepを繰り返す
		while (std::chrono::steady_clock::now() - referense_ < kMinTime)
		{
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//現在の時間を記録する
	referense_ = std::chrono::steady_clock::now();
}
