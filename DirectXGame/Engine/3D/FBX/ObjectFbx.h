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

public://�萔
	//�{�[���̍ő吔
	static const int MAX_BONES = 32;

public://�T�u�N���X
	
	//�萔�o�b�t�@�\����(���W�ϊ��s��)
	struct ConstBufferDataTransform
	{
		XMMATRIX viewProj;		//�r���v���s��
		XMMATRIX world;			//���[���h�s��
		XMFLOAT3 cameraPos;		//�J�������W
	};
	//�X�L�j���O���
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};


public://�ÓI�����o�֐�
	//�ÓI������
	static void StaticInitialize(ID3D12Device* device);
	// �`��O����
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	// �`��㏈��
	static void PostDraw();
	// 3D�I�u�W�F�N�g����
	static ObjectFbx* Create();

	//�Z�b�^�[
	static void SetDevice(ID3D12Device* device) { ObjectFbx::device_ = device; }
	
private:
	//�O���t�B�b�N�X�p�C�v���C������
	static void CreateGraphicsPipeline();

private://�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device_;
	
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList_;
	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature_;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelineState_;

public://�����o�֐�

	//������
	bool Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw();
	//�A�j���[�V�����J�n
	void PlayAnimation();

protected://�����o�ϐ�
	//���[�J���X�P�[��
	XMFLOAT3 scale_ = { 1.0f,1.0f,1.0f };
	// ���[�J����]�p
	XMFLOAT3 rotation_ = { 0.0f,0.0f,0.0f };
	// ���[�J�����W
	XMFLOAT3 position_ = { 0.0f,0.0f,0.0f };
	// ���[�J�����[���h�s��
	XMMATRIX matWorld_ = {};
	// ���f��
	ModelFbx* modelF_ = nullptr;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBufferTransform;
	//�X�L��
	ComPtr<ID3D12Resource> constBufferSkin;

	//1�t���[���̎���
	FbxTime frameTime;
	// �A�j���[�V�����J�n����
	FbxTime startTime;
	// �A�j���[�V�����I������
	FbxTime endTime;
	//�A�j���[�V�������ݎ���
	FbxTime currentTime;
	//�A�j���[�V�����Đ����t���O
	bool isPlayAnimation = false;

	//�J����
	Camera* camera_ = nullptr;
public://�A�N�Z�b�T�u����
	void SetModelFBX(ModelFbx* modelF) { this->modelF_ = modelF; }
	void SetCamera(Camera* camera) { this->camera_ = camera; }

};
