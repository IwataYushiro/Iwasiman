#pragma once
#include "ModelFbx.h"
#include "Camera.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include <Windows.h>
#include <wrl.h>

class ObjectFbx
{
protected://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�T�u�N���X
	//�萔�o�b�t�@�\����(���W�ϊ��s��)
	struct ConstBufferDataTransform
	{
		XMMATRIX viewProj;		//�r���v���s��
		XMMATRIX world;			//���[���h�s��
		XMFLOAT3 cameraPos;		//�J�������W
	};

public://�ÓI�����o�֐�
	//�Z�b�^�[
	static void SetDevice(ID3D12Device* device) { ObjectFbx::device_ = device; }
	static void SetCamera(Camera* camera) { ObjectFbx::camera_ = camera; }
	//�O���t�B�b�N�X�p�C�v���C������
	static void CreateGraphicsPipeline();

private://�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device_;
	//�J����
	static Camera* camera_;
	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature_;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelineState_;
public://�����o�֐�

	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);


protected://�����o�ϐ�
	//���[�J���X�P�[��
	XMFLOAT3 scale_ = { 1.0f,1.0f,1.0f };
	// ���[�J����]�p
	XMFLOAT3 rotation_ = { 0.0f,0.0f,0.0f };
	// ���[�J�����W
	XMFLOAT3 position_ = { 0.0f,0.0f,0.0f };
	// ���[�J�����[���h�s��
	XMMATRIX matWorld_;
	// ���f��
	ModelFbx* modelF_ = nullptr;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBufferTransform;
public://�A�N�Z�b�T�u����
	void SetModelFBX(ModelFbx* modelF) { this->modelF_ = modelF; }
};
