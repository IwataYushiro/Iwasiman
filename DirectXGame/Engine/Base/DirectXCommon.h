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

*	DirectX��ՃN���X

*/
class DirectXCommon final
{
public: // �ȗ�
	//�G�C���A�X�e���v���[�g
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://�V���O���g���C���X�^���X
	static DirectXCommon* GetInstance();
public:
	
	//������(Windows�A�v���P�[�V����)
	void Initialize(const WinApp* winApp);
	//�f�o�C�X�̏�����
	void InitializeDevice();
	//�R�}���h�֘A�̏�����
	void InitializeCommand();
	//�X���b�v�`�F�[���̏�����
	void InitializeSwapchain();
	//�����_�[�^�[�Q�b�g�r���[�̏�����
	void InitializeRenderTargetView();
	//�[�x�o�b�t�@�̏�����
	void InitializeDepthBuffer();
	//�[�x�r���[�쐬
	void CreateDepthView();
	//�t�F���X������
	void InitializeFence();

	//�`��O����
	void PreDraw();
	//�`��㏈��
	void PostDraw();
private:
	//DirectX12�f�o�C�X
	ComPtr<ID3D12Device> device_ = nullptr;
	//DXGI�t�@�N�g���[
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	//�R�}���h�A���P�[�^
	ComPtr<ID3D12CommandAllocator> cmdAllocator_ = nullptr;
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	//�X���b�v�`�F�[���f�X�N
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	
	//�����_�[�^�[�Q�b�g�r���[�f�X�N
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc_{};
	//�����_�[�^�[�Q�b�g�r���[�q�[�v
	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
	//�[�x�r���[�q�[�v
	ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff_ = nullptr;
	// �o�b�N�o�b�t�@
	std::vector<ComPtr<ID3D12Resource>> backBuffers_;
	// �t�F���X
	ComPtr<ID3D12Fence> fence_ = nullptr;
	//�t�F���X�l
	UINT64 fenceVal_ = 0;

	//Windows�A�v���P�[�V����
	const WinApp* winApp_ = nullptr;

	//���\�[�X�o���A
	D3D12_RESOURCE_BARRIER barrierDesc_{};

public://�A�N�Z�b�T�u����
	//�f�o�C�X�擾
	ID3D12Device* GetDevice() const { return device_.Get(); }
	//�R�}���h���X�g�擾
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	//�o�b�N�o�b�t�@�����擾
	size_t GetBuckBufferCount()const { return backBuffers_.size(); }
private://�����o�֐�
	//FPS�Œ菉����
	void InitializeFixFPS();
	//FPS�Œ�X�V
	void UpdateFixFPS();
	//�L�^����(FPS�Œ�p)
	std::chrono::steady_clock::time_point referense_;
private:
	//�R���X�g���N�^�i�V���O���g���p�^�[���j
	DirectXCommon() = default;
	//�f�X�g���N�^�i�V���O���g���p�^�[���j
	~DirectXCommon() = default;
public:
	//�R�s�[�R���X�g���N�^�̖h�~�i�V���O���g���p�^�[���j
	DirectXCommon(const DirectXCommon& obj) = delete;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	DirectXCommon& operator=(const DirectXCommon& obj) = delete;
};
