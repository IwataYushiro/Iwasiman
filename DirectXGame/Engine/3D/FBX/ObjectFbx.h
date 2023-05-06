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

private://�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device_;
	//�J����
	static Camera* camera_;

public://�����o�֐�

	//������
	void Initialize();

protected://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBufferTransform;

};
