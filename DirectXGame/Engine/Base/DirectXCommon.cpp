#include <DirectXCommon.h>

#include <cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;

/*

*	DirectXCommon.cpp

*	DirectX��ՃN���X

*/

DirectXCommon* DirectXCommon::GetInstance()
{
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize(WinApp* winApp)
{
	//NULL�`�F�b�N
	assert(winApp);

	this->winApp_ = winApp;

#pragma region DirectX������
	//FPS�Œ菉����
	InitializeFixFPS();
	
	//�f�o�C�X�̏�����
	InitializeDevice();
	//�R�}���h�֘A�̏�����
	InitializeCommand();
	//�X���b�v�`�F�[���̏�����
	InitializeSwapchain();
	//�����_�[�^�[�Q�b�g�r���[�̏�����
	InitializeRenderTargetView();
	//�[�x�o�b�t�@�̏�����
	InitializeDepthBuffer();
	//�t�F���X������
	InitializeFence();
#pragma endregion
}

void DirectXCommon::InitializeDevice()
{
#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	HRESULT result;

#pragma region �f�o�b�O���C��

#pragma endregion
#pragma region �A�_�v�^�̗�
	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//�A�_�v�^�[�񋓗p
	std::vector<ComPtr<IDXGIAdapter4>> adapters;
	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//�p�t�H�[�}���X���������̂��珇�ɁA�S�ẴA�_�v�^�[���
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		//���I�z��ɒǉ�
		adapters.push_back(tmpAdapter);
	}
#pragma endregion

#pragma region �A�_�v�^�̑I��
	//�Ó��ȃA�_�v�^��I��
	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		//�A�_�v�^�̏����擾
		adapters[i]->GetDesc3(&adapterDesc);

		//�\�t�g�E�F�A�f�o�C�X�����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//�f�o�C�X���̗p���ă��[�v�𔲂���
			tmpAdapter = adapters[i].Get();
			break;
		}
	}
#pragma endregion

#pragma region �f�o�C�X�̐���
	// �Ή����x���̔z��
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
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result == S_OK) {
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion
}

void DirectXCommon::InitializeCommand()
{
	HRESULT result;

#pragma region �R�}���h���X�g�̐���
	// �R�}���h�A���P�[�^�𐶐�
	result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator_));
	assert(SUCCEEDED(result));

	// �R�}���h���X�g�𐶐�
	result = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region �R�}���h�L���[�̐���
	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//�R�}���h�L���[�𐶐�
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));
#pragma endregion
}

void DirectXCommon::InitializeSwapchain()
{
	HRESULT result;

#pragma region �X���b�v�`�F�[���̐���
	//�}�W�b�N�i���o�[�o�ŗp�̃v���Z�b�g
	struct SwapChainPreset
	{
		const UINT width = 1280;
		const UINT height = 720;
		const UINT sampleDescCount = 1;
		const UINT bufferCount = 2;
	};
	SwapChainPreset preset;

	// �X���b�v�`�F�[���̐ݒ�
	swapChainDesc_.Width = preset.width;
	swapChainDesc_.Height = preset.height;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �F���̏���
	swapChainDesc_.SampleDesc.Count = preset.sampleDescCount; // �}���`�T���v�����Ȃ�
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER; // �o�b�N�o�b�t�@�p
	swapChainDesc_.BufferCount = preset.bufferCount; // �o�b�t�@����2�ɐݒ�
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // �t���b�v��͔j��
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// �X���b�v�`�F�[���̐���
	//IDXGISwapChain1��ComPtr��p��
	ComPtr<IDXGISwapChain1> swapChain1;

	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winApp_->GetHwnd(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1);

	//��������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�
	swapChain1.As(&swapChain_);

	assert(SUCCEEDED(result));
#pragma endregion
}

void DirectXCommon::InitializeRenderTargetView()
{

#pragma region �����_�r���[�^�[�Q�b�g
	// �f�X�N���v�^�q�[�v�̐ݒ�

	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount; // ���\��2��
	// �f�X�N���v�^�q�[�v�̐���
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));

	// �o�b�N�o�b�t�@
	backBuffers_.resize(swapChainDesc_.BufferCount);

	// �X���b�v�`�F�[���̑S�Ẵo�b�t�@�ɂ��ď�������
	for (size_t i = 0; i < backBuffers_.size(); i++) {
		// �X���b�v�`�F�[������o�b�t�@���擾
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));
		// �f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		// �����\���ŃA�h���X�������
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		// �V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// �����_�[�^�[�Q�b�g�r���[�̐���
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}

#pragma endregion
}

void DirectXCommon::InitializeDepthBuffer()
{

	HRESULT result;
	//�v���Z�b�g
	const UINT16 depthOrArraySize = 1;
	const UINT sampleDescCount = 1;
	//�[�x�o�b�t�@�ݒ�
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = winApp_->WINDOW_WIDTH;		//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResourceDesc.Height = winApp_->WINDOW_HEIGHT;	//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResourceDesc.DepthOrArraySize = depthOrArraySize;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g
	depthResourceDesc.SampleDesc.Count = sampleDescCount;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//�f�v�X�X�e���V��

	//�[�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�l�̃N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	const float defaultDepthNum = 1.0f;
	depthClearValue.DepthStencil.Depth = defaultDepthNum;//�[�x�l1.0f(�ő�l)�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g

	//�[�x�o�b�t�@����

	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//�[�x�l�������݂ɕK�v
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));

	//�[�x�r���[�p�f�X�N���v�^�q�[�v����
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	const UINT descriptorNum = 1;
	dsvHeapDesc.NumDescriptors = descriptorNum;//�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; //�f�v�X�X�e���V���r���[

	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
	CreateDepthView();

}
void DirectXCommon::CreateDepthView()
{
	//�[�x�r���[�̍쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; //�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,
		dsvHeap_->GetCPUDescriptorHandleForHeapStart());

}
void DirectXCommon::InitializeFence()
{
	HRESULT result;
#pragma region �t�F���X����

	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
#pragma endregion
}

void DirectXCommon::PreDraw()
{
	// �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	// 1.���\�[�X�o���A�ŏ������݉\�ɕύX

	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get(); // �o�b�N�o�b�t�@���w��
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
	const UINT barrierNum = 1;
	commandList_->ResourceBarrier(barrierNum, &barrierDesc_);

	// 2.�`���̕ύX
	const int RTVNum = 1;//�����_�[�^�[�Q�b�g�r���[�̐�
	// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += static_cast<unsigned long long>(bbIndex) * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 3.��ʃN���A			R	  G		B	A
	const FLOAT clearColor[] = { 0.25f,0.25f,0.25f,0.0f }; //���ۂ��F
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	//�[�x�o�b�t�@�N���A
	const float depthNum = 1.0f;
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, depthNum, 0, 0, nullptr);

	// 4.�`��R�}���h��������
	//�r���[�|�[�g�ݒ�R�}���h
	const float maxDepthNum = 1.0f;

	D3D12_VIEWPORT viewport{};
	viewport.Width = WinApp::WINDOW_WIDTH;
	viewport.Height = WinApp::WINDOW_HEIGHT;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = maxDepthNum;
	//�r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList_->RSSetViewports(RTVNum, &viewport);

	//�V�U�[��`
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + WinApp::WINDOW_WIDTH;
	scissorRect.top = 0;
	scissorRect.bottom = scissorRect.top + WinApp::WINDOW_HEIGHT;
	//�V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList_->RSSetScissorRects(RTVNum, &scissorRect);
}

void DirectXCommon::PostDraw()
{
	HRESULT result;

	// 5.���\�[�X�o���A��߂�
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//�`���Ԃ���
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			//�\����Ԃ�
	commandList_->ResourceBarrier(1, &barrierDesc_);

	// �R�}���h�̃t���b�V��

	// ���߂̃N���[�Y
	result = commandList_->Close();
	assert(SUCCEEDED(result));
	// �R�}���h���X�g�̎��s
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
	const UINT syncInterval = 1;
	result = swapChain_->Present(syncInterval, 0);
	assert(SUCCEEDED(result));
	//FPS�Œ�
	UpdateFixFPS();

	//�R�}���h�̎��s������҂�
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//�L���[���N���A
	result = cmdAllocator_->Reset();
	assert(SUCCEEDED(result));
	//�ĂуR�}���h���X�g�𒙂߂鏀��
	result = commandList_->Reset(cmdAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}

void DirectXCommon::InitializeFixFPS()
{
	referense_ = std::chrono::steady_clock::now();
}

void DirectXCommon::UpdateFixFPS()
{
	//1/60�b�҂�����̎���
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	//1/60�b���킸���ɒZ������
	const std::chrono::microseconds kMinChackTime(uint64_t(1000000.0f / 65.0f));

	//���ݎ��Ԃ��擾����
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - referense_);

	//1/60�b(���킸���ɒZ������)�o���Ă��Ȃ��ꍇ
	if (elapsed < kMinChackTime)
	{
		//1/60�b�o�߂���܂Ŕ�����sleep���J��Ԃ�
		while (std::chrono::steady_clock::now() - referense_ < kMinTime)
		{
			//1�}�C�N���b�X���[�v
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//���݂̎��Ԃ��L�^����
	referense_ = std::chrono::steady_clock::now();
}
