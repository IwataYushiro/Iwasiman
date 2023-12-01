#pragma once
#include <DirectXMath.h>
#include <memory>
#include "DirectionalLight.h"
#include "PointLight.h"

/*

*	LightGroup.h

*	���C�g�O���[�v

*/
class LightGroup
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://�萔
	static const int DirLightNum = 3;
	//�_�����̐�
	static const int PointLightNum = 3;

public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		//�����̐F
		XMFLOAT3 ambientColor;
		float pad1;
		//���s�����p
		DirectionalLight::ConstBufferData dirLights[DirLightNum];
		//�_�����p
		PointLight::ConstBufferData pointLights[PointLightNum];
	};

private://�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device_;

public://�ÓI�����o�֐�
	//�ÓI������(�f�o�C�X)
	static void StaticInitialize(ID3D12Device* device);
	//�C���X�^���X����
	static std::unique_ptr<LightGroup> Create();

private://�����o�ϐ�
	
	//�����̐F
	const XMFLOAT3 presetAmbientColor_ = { 1.0f,1.0f,1.0f };//�v���Z�b�g
	XMFLOAT3 ambientColor_ = presetAmbientColor_;
	//���s�����̔z��
	DirectionalLight dirLights_[DirLightNum];
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff_;
	//�_�[�e�B�t���O
	bool dirty_ = false;

	//�_�����̔z��
	PointLight pointLights_[PointLightNum];

public://�����o�֐�
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��(�R�}���h���X�g�A���[�g�p�����[�^�C���f�b�N�X)
	void Draw(ID3D12GraphicsCommandList* cmdList, const UINT rootParameterIndex);

	//�萔�o�b�t�@�]��
	void TransferConstBuffer();
	//�W���̃��C�g�ݒ�
	void DefaultLightSetting();

	//�����̃��C�g�F���Z�b�g(�J���[)
	void SetAmbientColor(const XMFLOAT3& color);
	//���s�����̗L���t���O�̃Z�b�g(���ԃ��C�g�A�N���t���O)
	void SetDirLightActive(const int index, const bool active);
	//���s�����̃��C�g�����̃Z�b�g(���ԃ��C�g�A����)
	void SetDirLightDir(const int index, const XMVECTOR& lightdir);
	//���s�����̃��C�g�F�̃Z�b�g(���ԃ��C�g�A�J���[)
	void SetDirLightColor(const int index, const XMFLOAT3& lightcolor);

	//�_����
	//���C�g���W�Z�b�g(���ԃ��C�g�A���W)
	void SetPointLightPos(const int index,const XMFLOAT3& lightPos);
	//���C�g�̐F�Z�b�g(���ԃ��C�g�A�J���[)
	void SetPointLightColor(const int index, const XMFLOAT3& lightColor);
	//���C�g�����̌����W���Z�b�g(���ԃ��C�g�A�����W��)
	void SetPointLightAtten(const int index, const XMFLOAT3& lightAtten);
	//�_�����̃��C�g�Z�b�g(���ԃ��C�g�A�N���t���O)
	void SetPointLightActive(const int index, const bool active);
};


