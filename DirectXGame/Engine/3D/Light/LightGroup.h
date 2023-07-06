#pragma once
#include <DirectXMath.h>
#include "DirectionalLight.h"

//���C�g
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

public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		//�����̐F
		XMFLOAT3 ambientColor;
		float pad1;
		//���s�����p
		DirectionalLight::ConstBufferData dirLights[DirLightNum];
	};

private://�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device;

public://�ÓI�����o�֐�
	//�ÓI������
	static void StaticInitialize(ID3D12Device* device);
	//�C���X�^���X����
	static LightGroup* Create();

private://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//�����̐F
	XMFLOAT3 ambientColor = { 1.0f,1.0f,1.0f };
	//���s�����̔z��
	DirectionalLight dirLights[DirLightNum];
	//�_�[�e�B�t���O
	bool dirty = false;

public://�����o�֐�
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	//�萔�o�b�t�@�]��
	void TransferConstBuffer();
	//�W���̃��C�g�ݒ�
	void DefaultLightSetting();

	//�����̃��C�g�F���Z�b�g
	void SetAmbientColor(const XMFLOAT3& color);
	//���s�����̗L���t���O�̃Z�b�g
	void SetDirLightActive(int index, bool active);
	//���s�����̃��C�g�����̃Z�b�g
	void SetDirLightDir(int index, const XMVECTOR& lightdir);
	//���s�����̃��C�g�F�̃Z�b�g
	void SetDirLightColor(int index, const XMFLOAT3& lightcolor);
};


