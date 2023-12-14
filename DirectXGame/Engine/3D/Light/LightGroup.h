#pragma once
#include <DirectXMath.h>
#include <memory>
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
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
		enum LightNum
		{
			LN_0,	//0�ԃ��C�g
			LN_1,	//1�ԃ��C�g
			LN_2,	//2�ԃ��C�g
			LN_NUM,	//�z��p
		};
		//���C�g��
		static const int32_t DirLightNum = 3;
		//�_�����̐�
		static const int32_t PointLightNum = 3;
		//�X�|�b�g���C�g�̐�
		static const int32_t SpotLightNum = 3;
		//�ۉe�̐�
		static const int32_t CircleShadowNum = 1;

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
			//�X�|�b�g���C�g�p
			SpotLight::ConstBufferData spotLights[SpotLightNum];
			//�ۉe�p
			CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
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

		//�X�|�b�g���C�g�̔z��
		SpotLight spotLights_[SpotLightNum];

		//�ۉe�̔z��
		CircleShadow circleShadows_[CircleShadowNum];

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
		void SetPointLightPos(const int index, const XMFLOAT3& lightPos);
		//���C�g�̐F�Z�b�g(���ԃ��C�g�A�J���[)
		void SetPointLightColor(const int index, const XMFLOAT3& lightColor);
		//���C�g�����̌����W���Z�b�g(���ԃ��C�g�A�����W��)
		void SetPointLightAtten(const int index, const XMFLOAT3& lightAtten);
		//�_�����̃��C�g�Z�b�g(���ԃ��C�g�A�N���t���O)
		void SetPointLightActive(const int index, const bool active);

		//�X�|�b�g���C�g
		//���C�g�����Z�b�g(���ԃ��C�g�A����)
		void SetSpotLightDir(const int index, const XMVECTOR& lightDir);
		//���C�g���W�Z�b�g(���ԃ��C�g�A���W)
		void SetSpotLightPos(const int index, const XMFLOAT3& lightPos);
		//���C�g�̐F�Z�b�g(���ԃ��C�g�A�J���[)
		void SetSpotLightColor(const int index, const XMFLOAT3& lightColor);
		//���C�g�����̌����W���Z�b�g(���ԃ��C�g�A�����W��)
		void SetSpotLightAtten(const int index, const XMFLOAT3& lightAtten);
		//���C�g�����p�x�Z�b�g(���ԃ��C�g�A�����p�x)
		void SetSpotLightFactorAngleCos(const int index, const XMFLOAT2& lightFactorAngleCos);
		//�X�|�b�g���C�g�̃��C�g�Z�b�g(���ԃ��C�g�A�N���t���O)
		void SetSpotLightActive(const int index, const bool active);

		//�ۉe
		//�ۉe�̕����Z�b�g(���ԃV���h�E�A����)
		void SetCircleShadowDir(const int index, const XMVECTOR& lightDir);
		//�ۉe�̃L���X�^�[���W�Z�b�g(���ԃV���h�E�A���W)
		void SetCircleShadowCasterPos(const int index, const XMFLOAT3& casterPos);
		//�ۉe�̃L���X�^�[�ƃ��C�g�̋����Z�b�g(���ԃV���h�E�A����)
		void SetCircleShadowDistanceCasterLight(const int index, const float distanceCasterLight);
		//�ۉe�̌����W���Z�b�g(���ԃV���h�E�A�����W��)
		void SetCircleShadowAtten(const int index, const XMFLOAT3& lightAtten);
		//�ۉe�̌����p�x�Z�b�g(���ԃV���h�E�A�����p�x)
		void SetCircleShadowFactorAngleCos(const int index, const XMFLOAT2& lightFactorAngleCos);
		//�ۉe�Z�b�g(���ԃV���h�E�A�N���t���O)
		void SetCircleShadowActive(const int index, const bool active);
	};

}