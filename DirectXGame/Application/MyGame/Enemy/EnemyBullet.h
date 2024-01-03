#pragma once
#include "Model.h"
#include "Object3d.h"
#include "ParticleManager.h"
#include <DirectXMath.h>

/*

*	EnemyBullet.h

*	�G�̒e

*/
class EnemyBullet: public IwasiEngine::Object3d {
private:
	// DirectX::���ȗ�
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::���ȗ�
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using CollisionInfo = IwasiEngine::CollisionInfo;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;

public:
	//����(�������W�A���x�A�g�p���f��)
	static std::unique_ptr<EnemyBullet> Create(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model = nullptr);
	//������(�������W�A���x)
	bool Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity);
	//���Z�b�g����
	void Reset();

	//�X�V
	void Update()override;
	//�`��
	void Draw();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	//���x
	XMFLOAT3 velocity_;

	//����
	static const int32_t lifeTime_ = 60 * 2;
	//���S����
	int32_t deathTimer_ = lifeTime_;
	//���S�t���O
	bool isDead_ = false;
	//���a
	float radius_ = 4.0f;

public: //�A�N�Z�b�T�A�C�����C���֐�
	//���񂾂��ǂ���
	bool IsDead() const { return isDead_; }

};