#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

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
	//�ÓI������
	static void StaticInitialize(ID3D12Device* device);

private://�ÓI�����o�ϐ�
	static ID3D12Device* device_;

public://���L�����o�ϐ�
	std::string name;				//�}�e���A����
	XMFLOAT3 ambient;				//�A���r�G���g�e���x
	XMFLOAT3 diffuse;				//�f�B�t���[�Y�e���x
	XMFLOAT3 specular;				//�X�y�L�����[�e���x
	float alpha;					//�A���t�@�l
	std::string textureFilename;	//�e�N�X�`���t�@�C����

public://�����o�֐�	
	
	// �e�N�X�`���ǂݍ���
	void LoadTexture(
		const std::string& directoryPath, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	// �X�V
	void Update();

private://�����o�ϐ�
	ComPtr<ID3D12Resource> constBuff; // �萔�o�b�t�@
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	//�}�b�s���O
	ConstBufferDataB1* constMap = nullptr;

private://�J�v�Z���������o�֐�
	//�R���X�g���N�^
	Material() {
		ambient = { 0.3f,0.3f,0.3f };
		diffuse = { 0.0f,0.0f,0.0f };
		specular = { 0.0f,0.0f,0.0f };
		alpha = 1.0f;
		cpuDescHandleSRV = {};
		gpuDescHandleSRV = {};
	}

	//������
	void Initialize();
	//�萔�o�b�t�@����
	void CreateConstBuffer();

public://�A�N�Z�b�T�u����
	// �萔�o�b�t�@�̎擾
	ID3D12Resource* GetConstantBuffer() { return constBuff.Get(); }
	//�n���h���֌W
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return gpuDescHandleSRV; }


};
