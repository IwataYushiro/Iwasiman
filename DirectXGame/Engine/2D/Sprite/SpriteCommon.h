#pragma once
#include <array>
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
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

	public://�\���́A�񋓑�
		//���[�g�p�����[�^�C���f�b�N�X
		enum RootParameterIndex
		{
			RPI_ConstBuffMaterial = 0,
			RPI_TexBuff = 1,
			RPI_ConstBuffTransform = 2,
			RPI_Num = 3,
		};

	public://�����o�֐�

		//������(DirectX���)
		void Initialize();

		//�e�N�X�`���ǂݍ���(�e�N�X�`���C���f�b�N�X�A�g�p�X�v���C�g)
		void LoadTexture(const uint32_t index, const std::string& fileName);

		//�`��O����
		void PreDraw();

		//�`��p�e�N�X�`���R�}���h�̔��s(�e�N�X�`���C���f�b�N�X)
		void SetTextureCommands(const uint32_t index);

	private://�����o�ϐ�

		//DirectX���
		const DirectXCommon* dxCommon_ = nullptr;

		//�f�t�H���g�e�N�X�`���i�[�f�B���N�g��
		static std::string kDefaultTextureDirectoryPath_;

		//SRV�̍ő��
		static const size_t kMaxSRVCount_ = 2056;

		//�e�N�X�`���o�b�t�@�̐���
		std::array<ComPtr<ID3D12Resource>, kMaxSRVCount_> texBuffs_;

		//�T�C�Y�̖₢���킹
		UINT incrementSize_;

		//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
		ComPtr<ID3D12DescriptorHeap> srvHeap_;
		//SRV�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_;
		//SRVGPU�q�[�v�̃n���h�����擾
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_;


		//�V�F�[�_�I�u�W�F�N�g
		ComPtr<ID3DBlob> vsBlob_ = nullptr;		//���_�V�F�[�_�[�I�u�W�F�N�g
		ComPtr<ID3DBlob> psBlob_ = nullptr;		//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
		ComPtr<ID3DBlob> errorBlob_ = nullptr;	//�G���[�I�u�W�F�N�g

		//�p�C�v���C���X�e�[�g�̐���
		ComPtr<ID3D12PipelineState> pipelineState_;
		//���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootSignature_;
		// ���[�g�V�O�l�`���̃V���A���C�Y
		ComPtr<ID3DBlob> rootSigBlob_ = nullptr;

		//�f�B���N�g���p�X
		std::wstring directoryPath_;
		//�t�@�C����
		std::wstring fileName_;
		//�t�@�C���g���q
		std::wstring fileExt_;
	public://�A�N�Z�b�T

		//DirectX��ՃQ�b�g
		const DirectXCommon* GetDxCommon() const { return dxCommon_; }

		//SRV�q�[�v�Q�b�g
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return srvHandle_; }
		//SRVGPU�q�[�v�Q�b�g
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() { return srvGpuHandle_; }
		//�e�N�X�`���o�b�t�@�Q�b�g
		ID3D12Resource* GetTextureBuffer(const uint32_t index)const { return texBuffs_[index].Get(); }
		//�p�C�v���C���X�e�[�g�Q�b�g
		ID3D12PipelineState* GetPipelineState() { return pipelineState_.Get(); }
		//���[�g�V�O�l�`���Q�b�g
		ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	private:
		//�t�H���_�p�X�ƃt�@�C�����𕪗�
		void SeparateFilePath(const std::wstring& filePath);
	private:
		//�R���X�g���N�^�i�V���O���g���p�^�[���j
		SpriteCommon() = default;
		//�f�X�g���N�^�i�V���O���g���p�^�[���j
		~SpriteCommon() = default;
	public:
		//�R�s�[�R���X�g���N�^�̖h�~�i�V���O���g���p�^�[���j
		SpriteCommon(const SpriteCommon& obj) = delete;
		// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
		SpriteCommon& operator=(const SpriteCommon& obj) = delete;
	};
}