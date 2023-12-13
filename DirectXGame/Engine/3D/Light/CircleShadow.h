#pragma once
#include <DirectXMath.h>
/*

*	CircleShadow.h (cpp X)

*	�ۉe

*/
class CircleShadow
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
		XMVECTOR dir;
		XMFLOAT3 casterPos;
		float distanceCasterLight;
		XMFLOAT3 atten;
		float pad;
		XMFLOAT2 factorAngleCos;
		unsigned int active;
		float pad2;

	};
private://�����o�ϐ�
	//����(�P�ʃx�N�g��)
	const XMVECTOR presetDir_ = { 1.0f,0.0f,0.0f,0.0f };//�v���Z�b�g
	XMVECTOR dir_ = presetDir_;
	//�L���X�^�[�ƃ��C�g�̋���
	const float presetDistanceCasterLight_ = 300.0f;//�v���Z�b�g
	float distanceCasterLight_ = presetDistanceCasterLight_;
	//�L���X�^�[���W(���[���h���W�n)
	const XMFLOAT3 presetCasterPos_ = { 0.0f,0.0f,0.0f };//�v���Z�b�g
	XMFLOAT3 casterPos_ = presetCasterPos_;
	
	//���������W��
	const XMFLOAT3 presetAtten_ = { 0.5f,0.6f,0.0f };//�v���Z�b�g
	XMFLOAT3 atten_ = presetAtten_;
	//�����p�x
	const XMFLOAT2 presetFactorAngleCos_ = { 0.2f,0.5f };//�v���Z�b�g
	XMFLOAT2 factorAngleCos_ = presetFactorAngleCos_;
	//�L���t���O
	bool active_ = false;

public://�����o�֐�
	//�����Z�b�g(����)
	inline void SetDir(const XMVECTOR& dir){this->dir_ = DirectX::XMVector3Normalize(dir);}
	//�����Q�b�g
	inline const XMVECTOR& GetDir() { return dir_; }
	//�L���X�^�[���W�Z�b�g(���W)
	inline void SetCasterPos(const XMFLOAT3& casterPos) { this->casterPos_ = casterPos; }
	//�L���X�^�[���W�Q�b�g
	inline const XMFLOAT3& GetCasterPos() { return casterPos_; }
	//�L���X�^�[�ƃ��C�g�̋����Z�b�g(����)
	inline void SetDistanceCasterLight(const float distanceCasterLight)
	{ this->distanceCasterLight_ = distanceCasterLight; }
	//�L���X�^�[�ƃ��C�g�̋����Q�b�g
	inline const float GetDistanceCasterLight() { return distanceCasterLight_; }
	//�����W���Z�b�g(�����W��)
	inline void SetAtten(const XMFLOAT3& atten) { this->atten_ = atten; }
	//�����W���Q�b�g
	inline const XMFLOAT3& GetAtten() { return atten_; }
	//�����p�x�Z�b�g(�����p�x)
	inline void SetFactorAngleCos(const XMFLOAT2& factorAngle)
	{
		this->factorAngleCos_.x = cosf(DirectX::XMConvertToRadians(factorAngle.x));
		this->factorAngleCos_.y = cosf(DirectX::XMConvertToRadians(factorAngle.y));
	}
	//�����p�x�Q�b�g
	inline const XMFLOAT2& GetFactorAngleCos() { return factorAngleCos_; }
	//�ۉe�Z�b�g(�L�����t���O)
	inline void SetActive(const bool active) { this->active_ = active; }
	//�ۉe�Q�b�g
	inline bool IsActive() { return active_; }
};
