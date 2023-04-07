#pragma once

#include <cassert>
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCommon.h>
#include <Input.h>
#include <random>
#include <wrl.h>
#include <d3dcompiler.h>
#include <string>
#include <math.h>
#include <DirectXTex.h>
#include "WinApp.h"

//using namespace DirectX;


class Mesh
{
public: // �����o�֐�
	//�G�C���A�X�e���v���[�g�Ƃ��ŐF�X�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
private:
	//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;	//�F(RGBA)

	};
	struct ConstBufferDataTransform
	{
		XMMATRIX mat; // 3D�ϊ��s��

	};

	struct Material3d
	{
		//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
		ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

		//�\���̂�ϐ���
		ConstBufferDataMaterial* cbdm = nullptr;

		//�}�b�s���O�p�̃|�C���^
		ConstBufferDataMaterial* constMapMaterial = nullptr;

	};
	//3D�I�u�W�F�N�g�^
	struct Object3d
	{
		//�萔�o�b�t�@(�s��p)
		ComPtr<ID3D12Resource> constBuffTransform = nullptr;
		//�\���̂�ϐ���
		ConstBufferDataTransform* cbdt = nullptr;
		//�萔�o�b�t�@�}�b�v(�s��p)
		ConstBufferDataTransform* constMapTransform = nullptr;
		//�A�t�B���ϊ����
		XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
		XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
		XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		//���[���h�ϊ��s��
		XMMATRIX matWorld = DirectX::XMMatrixIdentity();
		//�e�I�u�W�F�N�g�ւ̃|�C���^
		Object3d* parent = nullptr;
	};

public:
	Mesh();
	~Mesh();
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	
	//�萔�o�b�t�@�̐ݒ�
	void CreateConstBufferMaterial3d(Material3d* material, ID3D12Device* device);
	void CreateConstBufferObject3d(Object3d* object, ID3D12Device* device);
	//
	//void CreateTextureBuffer(ID3D12Device* device, D3D12_RESOURCE_DESC resDesc);
	//�I�u�W�F�N�g�̏�����
	void SetObject3ds(int num);

	/// <summary>
	/// ���t���[������
	
	/// </summary>

	//��ʃN���A�ݒ�

	void Update(ID3D12Device* device);
	void UpdateObject3d(Object3d* object,XMMATRIX& matview, XMMATRIX& matprojection);
	void ControlObject3d(Object3d* object);
	/// <summary>
	/// �`��
	/// </summary>
	void Draw();
	//�����I�u�W�F�N�g�`��p
	void DrawObject3d(Object3d* object,
		D3D12_VERTEX_BUFFER_VIEW& vdView,D3D12_INDEX_BUFFER_VIEW& idView,UINT numIndices);

private://�����o�ϐ�
	//�C���v�b�g
	Input* input_ = nullptr;
	//WinApp
	WinApp* winApp_ = nullptr;
	//DirectXCommon
	DirectXCommon* dxCommon_ = nullptr;

	static const size_t VerticesCount_ = 24;
	//�C���f�b�N�X��
	static const size_t indicesCount_ = 36;
	//�I�u�W�F�N�g��
	static const size_t kObjectCount_ = 50;
	//3D�}�e���A��
	Material3d material3d_;
	//3D�I�u�W�F�N�g�̔z��
	Object3d object3ds_[kObjectCount_];
	
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz���W
		XMFLOAT3 normal;	//�@���x�N�g��
		XMFLOAT2 uv;		//uv���W
	};

	//���_�f�[�^
	Vertex vertices_[VerticesCount_] = {
		//�O(Z�Œ�)
		// x	 y	  z		  �@��u	  v
		{{-5.0f ,-5.0f ,-5.0f},{},{0.0f,1.0f}},//����	X��-�ō��@Y��-�ŉ�
		{{-5.0f,  5.0f ,-5.0f},{},{0.0f,0.0f}},//����	X��+�ŉE�@Y��+�ŏ�
		{{ 5.0f ,-5.0f ,-5.0f},{},{1.0f,1.0f}},//�E��					
		{{ 5.0f , 5.0f ,-5.0f},{},{1.0f,0.0f}},//�E��	
		//��(�O�ʂ�Z���W�̕������t)
		// x	 y	  z		 u	  v
		{{-5.0f ,-5.0f ,5.0f},{},{0.0f,1.0f}},//����	X��-�ō��@Y��-�ŉ�
		{{-5.0f,  5.0f ,5.0f},{},{0.0f,0.0f}},//����	X��+�ŉE�@Y��+�ŏ�
		{{ 5.0f ,-5.0f ,5.0f},{},{1.0f,1.0f}},//�E��					
		{{ 5.0f , 5.0f ,5.0f},{}, {1.0f,0.0f}},//�E��

		//��(X�Œ�)
		// x	 y	  z		 u	  v
		{{-5.0f ,-5.0f ,-5.0f},{},{0.0f,1.0f}},//����	X��-�ō��@Y��-�ŉ�
		{{-5.0f ,5.0f ,-5.0f},{},{0.0f,0.0f}},//����	X��+�ŉE�@Y��+�ŏ�
		{{-5.0f ,-5.0f ,5.0f},{},{1.0f,1.0f}},//�E��					
		{{-5.0f , 5.0f , 5.0f},{},{1.0f,0.0f}},//�E��	
		//�E(���ʂ�X���W�̕������t)
		// x	 y	  z		 u	  v
		{{ 5.0f ,-5.0f ,-5.0f},{}, {0.0f,1.0f}},//����	X��-�ō��@Y��-�ŉ�
		{{ 5.0f ,5.0f , -5.0f},{},{0.0f,0.0f}},//����	X��+�ŉE�@Y��+�ŏ�
		{{ 5.0f ,-5.0f ,5.0f},{},{1.0f,1.0f}},//�E��					
		{{ 5.0f , 5.0f , 5.0f},{},{1.0f,0.0f}},//�E��

		//��(Y�Œ�)
		// x	 y	  z		 u	  v
		{{-5.0f ,-5.0f ,-5.0f},{},{0.0f,1.0f}},//����	X��-�ō��@Y��-�ŉ�
		{{-5.0f ,-5.0f , 5.0f},{},{0.0f,0.0f}},//����	X��+�ŉE�@Y��+�ŏ�
		{{ 5.0f ,-5.0f ,-5.0f},{},{1.0f,1.0f}},//�E��					
		{{ 5.0f ,-5.0f , 5.0f},{},{1.0f,0.0f}},//�E��	
		//��(���ʂ�Y���W�̕������t)
		// x	 y	  z		 u	  v
		{{-5.0f , 5.0f ,-5.0f},{},{0.0f,1.0f}},//����	X��-�ō��@Y��-�ŉ�
		{{-5.0f , 5.0f , 5.0f},{},{0.0f,0.0f}},//����	X��+�ŉE�@Y��+�ŏ�
		{{ 5.0f , 5.0f ,-5.0f},{},{1.0f,1.0f}},//�E��					
		{{ 5.0f , 5.0f , 5.0f},{}, {1.0f,0.0f}},//�E��
	};
	//���_�o�b�t�@�̐���
	ComPtr<ID3D12Resource> vertBuff = nullptr;

	//�C���f�b�N�X�f�[�^
	unsigned short indices[indicesCount_] =
	{
		//�O
		0,1,2,//�O�p�`1��
		2,1,3,//�O�p�`2��

		//��(�O�̖ʂ�4���Z)
		6,5,4,//�O�p�`3��
		7,5,6,//�O�p�`4��

		//��
		10,9,8,//�O�p�`5��
		10,11,9,//�O�p�`6��

		//�E
		14,12,13,//�O�p�`7��
		15,14,13,//�O�p�`8��

		//��
		18,17,16,//�O�p�`9��
		19,17,18,//�O�p�`10��

		//��
		22,20,21,//�O�p�`11��
		22,21,23,//�O�p�`12��

	};
	//�C���f�b�N�X�o�b�t�@�̐���
	ComPtr<ID3D12Resource> indexBuff = nullptr;

	//�ˉe�ϊ��s��
	XMMATRIX matprojection;

	//�r���[�ϊ��s��
	XMMATRIX matview;
	//���_
	XMFLOAT3 eye;
	//�����_
	XMFLOAT3 target;
	//������x�N�g��
	XMFLOAT3 up;

	//���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vdView{};

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW idView{};

	//�e�N�X�`���o�b�t�@�̐���
	ComPtr<ID3D12Resource> texbuff = nullptr;
	ComPtr<ID3D12Resource> texbuff2 = nullptr;

	//�e�N�X�`���o�b�t�@��i�߂�p
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	static const int textureCount = 2;
	UINT incrementSize;
	//�X�y�[�X�L�[�����������̃t���O
	bool isSpace;

	//�V�F�[�_�[�֌W
	ComPtr<ID3DBlob> vsBlob = nullptr;		//���_�V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob = nullptr;		//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	
	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	ComPtr<ID3D12DescriptorHeap> srvHeap;

	//GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	Vertex* vertMap = nullptr;

	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob = nullptr;

	//�p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;

	float angle = 0.0f;//�J�����̉�]�p
};
