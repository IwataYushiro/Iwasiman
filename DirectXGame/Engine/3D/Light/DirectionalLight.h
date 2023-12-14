#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	DirectionalLight.h

	*	���C�g

	*/
	class DirectionalLight
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

	public://�T�u�N���X
		//�萔�o�b�t�@�p�f�[�^�\����
		struct ConstBufferData
		{
			XMVECTOR lightV;		//���C�g�ւ̕�����\���x�N�g��
			XMFLOAT3 lightColor;	//���C�g�̐F
			unsigned int active;
		};

	private://�����o�ϐ�
		//���C�g��������(�P�ʃx�N�g��)
		const XMVECTOR presetLightDir_ = { 1.0f,0.0f,0.0f,0.0f };//�v���Z�b�g
		XMVECTOR lightDir_ = presetLightDir_;
		//���C�g�J���[
		const XMFLOAT3 presetLightColor_ = { 1.0f,1.0f,1.0f };//�v���Z�b�g
		XMFLOAT3 lightColor_ = presetLightColor_;
		//�L���t���O
		bool active_ = false;
		//�萔�o�b�t�@
		ComPtr<ID3D12Resource> constBuff_;


	public://�����o�֐�

		//���C�g�����Z�b�g
		void SetLightDir(const XMVECTOR& lightdir) { this->lightDir_ = DirectX::XMVector3Normalize(lightdir); }
		//���C�g�����Q�b�g
		const XMVECTOR& GetLightDir() { return lightDir_; }

		//���C�g�J���[�̃Z�b�g
		void SetLightColor(const XMFLOAT3& lightcolor) { this->lightColor_ = lightcolor; }
		//���C�g�J���[�̃Q�b�g
		const XMFLOAT3& GetLightColor() { return lightColor_; }

		//�L���t���O�̃Z�b�g
		inline void SetActive(bool active) { this->active_ = active; }
		//�L�����ǂ���
		inline bool IsActive() { return active_; }
	};
}
