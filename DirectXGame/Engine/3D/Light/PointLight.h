#pragma once
#include <DirectXMath.h>

//�|�C���g���C�g
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
	XMFLOAT3 lightPos_ = { 0.0f,0.0f,0.0f };
	//���C�g�̐F
	XMFLOAT3 lightColor_ = { 1.0f,1.0f,1.0f };
	//���C�g�����̌����W��
	XMFLOAT3 lightAtten_ = { 1.0f,1.0f,1.0f };
	//�A�N�e�B�u�t���O
	bool active_ = false;

public://�A�N�Z�b�T�u����
	
	//���C�g���W
	inline void SetLightPos(const XMFLOAT3& lightPos) { this->lightPos_ = lightPos; }
	inline const XMFLOAT3& GetLightPos() { return lightPos_; }
	//���C�g�̐F
	inline void SetLightColor(const XMFLOAT3& lightColor) { this->lightColor_ = lightColor; }
	inline const XMFLOAT3& GetLightColor() { return lightColor_; }
	//���C�g�����̌����W��
	inline void SetLightAtten(const XMFLOAT3& lightAtten) { this->lightAtten_ = lightAtten; }
	inline const XMFLOAT3& GetLightAtten() { return lightAtten_; }
	//���s�����̃��C�g�F�̃Z�b�g
	inline void SetActive(bool active) { this->active_ = active; }
	inline bool IsActive() { return active_; }
};