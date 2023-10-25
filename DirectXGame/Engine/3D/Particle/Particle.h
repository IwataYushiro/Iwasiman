#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <forward_list>

/*

*	Particle.h

*	�ꗱ�̃p�[�e�B�N��

*/
class Particle
{
private://�G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�T�u�N���X

	struct VertexPosScale
	{
		XMFLOAT3 pos; // xyz���W
		float scale;  //�X�P�[��
		XMFLOAT4 color;//�F
	};

	//�p�[�e�B�N���ꗱ
	struct OneParticle
	{
		//DirectX::���ȗ�
		using XMFLOAT3 = DirectX::XMFLOAT3;

		//���W
		XMFLOAT3 position = {};
		//���x
		XMFLOAT3 velocity = {};
		//�����x
		XMFLOAT3 accel = {};
		//����frame
		int frame = 0;
		//�I��frame
		int num_frame = 0;

		//�X�P�[��
		float scale = 1.0f;
		//�����l
		float s_scale = 1.0f;
		//�ŏI�l
		float e_scale = 0.0f;

		//�J���[
		XMFLOAT4 color = {};
	};

private://�萔
	static const int VERTEX_COUNT = 1024;		// ���_��

public://�����o�֐�

	//OBJ�t�@�C������p�[�e�B�N�����f����ǂݍ���(�g�p�p�[�e�B�N�����f��)
	static Particle* LoadFromParticleTexture(const std::string& fileName);
	// �f�X�N���v�^�q�[�v�̏�����
	void InitializeDescriptorHeap();
	// �e�N�X�`���ǂݍ���(�e�N�X�`���t�@�C���l�[��)
	void LoadTexture(const std::string& fileName);

	//�e��o�b�t�@����
	void CreateBuffers();
	//�X�V
	void Update();
	// �`��(�R�}���h���X�g)
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�p�[�e�B�N���̒ǉ�(�����A�������W�A���x�A�d�͉����x�A�J�n���X�P�[���A�I�����X�P�[��,�F)
	void Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel,
		float start_scale, float end_scale, XMFLOAT4 color = { 0.0f,1.0f,1.0f,1.0f });

private://�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device_;
	//�f�t�H���g�e�N�X�`���i�[�f�B���N�g��
	static std::string defaultTextureDirectoryPath_;

	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize_;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap_;
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff_;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	//���_�f�[�^�z��
	VertexPosScale vertices_[VERTEX_COUNT];

	std::forward_list<OneParticle> particles_;

public://�A�N�Z�b�T�u����
	//�f�o�C�X�Z�b�g
	static void SetDevice(ID3D12Device* device) { Particle::device_ = device; }

};
//�I�y���[�^�[
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
