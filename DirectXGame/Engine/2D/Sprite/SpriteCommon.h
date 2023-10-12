#pragma once
#include <array>
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>

/*

*	SpriteCommon.h

*	�X�v���C�g���

*/
class SpriteCommon final
{
public:
	//�G�C���A�X�e���v���[�g�Ƃ��ŐF�X�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�V���O���g���C���X�^���X
	static SpriteCommon* GetInstance();

public://�����o�֐�

	//������(DirectX���)
	void Initialize(DirectXCommon* dxCommon);

	//�e�N�X�`���ǂݍ���(�e�N�X�`���C���f�b�N�X�A�g�p�X�v���C�g)
	void LoadTexture(uint32_t index, const std::string& fileName);

	//�`��O����
	void PreDraw();

	//�`��p�e�N�X�`���R�}���h�̔��s(�e�N�X�`���C���f�b�N�X)
	void SetTextureCommands(uint32_t index);

private://�����o�ϐ�

	//DirectX���
	DirectXCommon* dxCommon_ = nullptr;

	//�f�t�H���g�e�N�X�`���i�[�f�B���N�g��
	static std::string kDefaultTextureDirectoryPath;

	//SRV�̍ő��
	static const size_t kMaxSRVCount = 2056;

	//�e�N�X�`���o�b�t�@�̐���
	std::array<ComPtr<ID3D12Resource>,kMaxSRVCount> texBuffs;

	//�T�C�Y�̖₢���킹
	UINT incrementSize;
	
	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	//SRV�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
	//SRVGPU�q�[�v�̃n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;


	//�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> vsBlob = nullptr;		//���_�V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob = nullptr;		//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	//�p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob = nullptr;

public://�A�N�Z�b�T

	//DirectX��ՃQ�b�g
	DirectXCommon* GetDxCommon() { return dxCommon_; }
	
	//SRV�q�[�v�Q�b�g
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return srvHandle; }
	//SRVGPU�q�[�v�Q�b�g
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() { return srvGpuHandle; }
	//�e�N�X�`���o�b�t�@�Q�b�g
	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return texBuffs[index].Get(); }
	//�p�C�v���C���X�e�[�g�Q�b�g
	ID3D12PipelineState* GetPipelineState() { return pipelineState.Get(); }
	//���[�g�V�O�l�`���Q�b�g
	ID3D12RootSignature* GetRootSignature() { return rootSignature.Get(); }
private:
	//�R���X�g���N�^
	SpriteCommon() = default;
	//�f�X�g���N�^
	~SpriteCommon() = default;
public:
	//�R�s�[�R���X�g���N�^�̖h�~
	SpriteCommon(const SpriteCommon& obj) = delete;
	SpriteCommon& operator=(const SpriteCommon& obj) = delete;
};
