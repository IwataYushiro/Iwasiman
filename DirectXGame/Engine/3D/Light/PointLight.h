#pragma once
#include <DirectXMath.h>

/*

*	PointLight.h (cpp X)

*	�|�C���g���C�g

*/
class PointLight
{
private: // �G�C���A�X
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
		XMFLOAT3 lightPos;
		float pad1;
		XMFLOAT3 lightColor;
		float pad2;
		XMFLOAT3 lightatten;
		unsigned int active;

	};

private://�����o�ϐ�
	//���C�g���W
	const XMFLOAT3 presetLightPos_ = { 0.0f,0.0f,0.0f };//�v���Z�b�g
	XMFLOAT3 lightPos_ = presetLightPos_;
	//���C�g�̐F
	const XMFLOAT3 presetLightColor_ = { 1.0f,1.0f,1.0f };//�v���Z�b�g
	XMFLOAT3 lightColor_ = presetLightColor_;
	//���C�g�����̌����W��
	const XMFLOAT3 presetLightAtten_ = { 1.0f,1.0f,1.0f };//�v���Z�b�g
	XMFLOAT3 lightAtten_ = presetLightAtten_;
	//�A�N�e�B�u�t���O
	bool active_ = false;

public://�A�N�Z�b�T�u����

	//���C�g���W�Z�b�g
	inline void SetLightPos(const XMFLOAT3& lightPos) { this->lightPos_ = lightPos; }
	//���C�g���W�Q�b�g
	inline const XMFLOAT3& GetLightPos() { return lightPos_; }
	//���C�g�J���[�Z�b�g
	inline void SetLightColor(const XMFLOAT3& lightColor) { this->lightColor_ = lightColor; }
	//���C�g�J���[�Q�b�g
	inline const XMFLOAT3& GetLightColor() { return lightColor_; }
	//���C�g�����̌����W���Z�b�g
	inline void SetLightAtten(const XMFLOAT3& lightAtten) { this->lightAtten_ = lightAtten; }
	//���C�g�����̌����W���Q�b�g
	inline const XMFLOAT3& GetLightAtten() { return lightAtten_; }
	//�_�����̃��C�g�Z�b�g
	inline void SetActive(const bool active) { this->active_ = active; }
	//�_�����̃��C�g�Q�b�g
	inline bool IsActive() { return active_; }
};