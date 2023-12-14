#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	Material.h

	*	�}�e���A���Ǘ�

	*/
	class Material
	{
	private: // �G�C���A�X
		// Microsoft::WRL::���ȗ�
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::���ȗ�
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;

	public://�T�u�N���X
		//�萔�o�b�t�@
		struct ConstBufferDataB1
		{
			XMFLOAT3 ambient;	// �A���r�G���g�W��
			float pad1;			// �p�f�B���O
			XMFLOAT3 diffuse;	// �f�B�t���[�Y�W��
			float pad2;			// �p�f�B���O
			XMFLOAT3 specular;	// �X�y�L�����[�W��
			float alpha;		// �A���t�@�l
		};

	public://�ÓI�����o�֐�
		//����
		static Material* Create();
		//�ÓI������(�f�o�C�X)
		static void StaticInitialize(ID3D12Device* device);

	private://�ÓI�����o�ϐ�
		static ID3D12Device* device_;

	private://�v���Z�b�g
		const XMFLOAT3 presetAmbient_ = { 0.3f,0.3f,0.3f };				//�A���r�G���g�e���x�̃v���Z�b�g
		const XMFLOAT3 presetDiffuse_ = { 0.0f,0.0f,0.0f };				//�f�B�t���[�Y�e���x�̃v���Z�b�g
		const XMFLOAT3 presetSpecular_ = { 0.0f,0.0f,0.0f };				//�X�y�L�����[�e���x�̃v���Z�b�g
		const float presetAlpha_ = 1.0f;								//�A���t�@�l�̃v���Z�b�g

	public://���L�����o�ϐ�
		std::string name;				//�}�e���A����
		XMFLOAT3 ambient;				//�A���r�G���g�e���x
		XMFLOAT3 diffuse;				//�f�B�t���[�Y�e���x
		XMFLOAT3 specular;				//�X�y�L�����[�e���x
		float alpha;					//�A���t�@�l
		std::string textureFilename;	//�e�N�X�`���t�@�C����

	public://�����o�֐�	

		// �e�N�X�`���ǂݍ���(�f�B���N�g���p�X�ACPU�n���h���AGPU�n���h��)
		void LoadTexture(
			const std::string& directoryPath, const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
			const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
		// �X�V
		void Update();

	private://�����o�ϐ�
		ComPtr<ID3D12Resource> constBuff_; // �萔�o�b�t�@
		// �e�N�X�`���o�b�t�@
		ComPtr<ID3D12Resource> texBuff_;
		// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
		// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
		//�}�b�s���O
		ConstBufferDataB1* constMap_ = nullptr;

	private://�J�v�Z���������o�֐�
		//�R���X�g���N�^
		Material() {
			ambient = presetAmbient_;
			diffuse = presetDiffuse_;
			specular = presetSpecular_;
			alpha = presetAlpha_;
			cpuDescHandleSRV_ = {};
			gpuDescHandleSRV_ = {};
		}

		//������
		void Initialize();
		//�萔�o�b�t�@����
		void CreateConstBuffer();

	public://�A�N�Z�b�T�u����
		// �萔�o�b�t�@�Q�b�g
		ID3D12Resource* GetConstantBuffer() { return constBuff_.Get(); }
		//�n���h���֌W
		//CPU�n���h���Q�b�g
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV_; }
		//GPU�n���h���Q�b�g
		const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return gpuDescHandleSRV_; }


	};
}