#pragma once
#include <DirectXMath.h>
namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	SpotLight.h (cpp X)

	*	�X�|�b�g���C�g

	*/
	class SpotLight
	{
	private://�G�C���A�X�e���v���[�g
		// DirectX::���ȗ�
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;

	public://�T�u�N���X
		//�萔�o�b�t�@�p�f�[�^�\����
		struct ConstBufferData
		{
			XMVECTOR lightv;
			XMFLOAT3 lightPos;
			float pad1;
			XMFLOAT3 lightColor;
			float pad2;
			XMFLOAT3 lightatten;
			float pad3;
			XMFLOAT2 lightfactoranglecos;
			unsigned int active;
			float pad4;

		};
	private://�����o�ϐ�
		//���C�g����(�P�ʃx�N�g��)
		const XMVECTOR presetLightDir_ = { 1.0f,0.0f,0.0f,0.0f };//�v���Z�b�g
		XMVECTOR lightDir_ = presetLightDir_;
		//���C�g���W(���[���h���W�n)
		const XMFLOAT3 presetLightPos_ = { 0.0f,0.0f,0.0f };//�v���Z�b�g
		XMFLOAT3 lightPos_ = presetLightPos_;
		//���C�g�F
		const XMFLOAT3 presetLightColor_ = { 1.0f,1.0f,1.0f };//�v���Z�b�g
		XMFLOAT3 lightColor_ = presetLightColor_;
		//���C�g���������W��
		const XMFLOAT3 presetLightAtten_ = { 1.0f,1.0f,1.0f };//�v���Z�b�g
		XMFLOAT3 lightAtten_ = presetLightAtten_;
		//���C�g�����p�x(�J�n�p�x�A�I���p�x)
		const XMFLOAT2 presetLightFactorAngleCos_ = { 0.5f,0.2f };//�v���Z�b�g
		XMFLOAT2 lightFactorAngleCos_ = presetLightFactorAngleCos_;
		//�L���t���O
		bool active_ = false;

	public://�����o�֐�
		//���C�g�����Z�b�g(����)
		inline void SetLightDir(const XMVECTOR& lightDir)
		{
			this->lightDir_ = DirectX::XMVector3Normalize(lightDir);
		}
		//���C�g�����Q�b�g
		inline const XMVECTOR& GetLightDir() { return lightDir_; }
		//���C�g���W�Z�b�g(���W)
		inline void SetLightPos(const XMFLOAT3& lightPos) { this->lightPos_ = lightPos; }
		//���C�g���W�Q�b�g
		inline const XMFLOAT3& GetLightPos() { return lightPos_; }
		//���C�g�J���[�Z�b�g(�J���[)
		inline void SetLightColor(const XMFLOAT3& lightColor) { this->lightColor_ = lightColor; }
		//���C�g�J���[�Q�b�g
		inline const XMFLOAT3& GetLightColor() { return lightColor_; }
		//���C�g�����̌����W���Z�b�g(�����W��)
		inline void SetLightAtten(const XMFLOAT3& lightAtten) { this->lightAtten_ = lightAtten; }
		//���C�g�����̌����W���Q�b�g
		inline const XMFLOAT3& GetLightAtten() { return lightAtten_; }
		//���C�g�����p�x�Z�b�g(�����p�x)
		inline void SetLightFactorAngleCos(const XMFLOAT2& lightFactorAngle)
		{
			this->lightFactorAngleCos_.x = cosf(DirectX::XMConvertToRadians(lightFactorAngle.x));
			this->lightFactorAngleCos_.y = cosf(DirectX::XMConvertToRadians(lightFactorAngle.y));
		}
		//���C�g�����p�x�Q�b�g
		inline const XMFLOAT2& GetLightFactorAngleCos() { return lightFactorAngleCos_; }
		//�X�|�b�g���C�g�̃��C�g�Z�b�g(�L�����t���O)
		inline void SetActive(const bool active) { this->active_ = active; }
		//�X�|�b�g���C�g�̃��C�g�Q�b�g
		inline bool IsActive() { return active_; }
	};
}