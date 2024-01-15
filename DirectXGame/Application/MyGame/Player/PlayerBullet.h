#pragma once
#include "Object3d.h"
#include "Model.h"
#include "ParticleManager.h"
#include <DirectXMath.h>

/*

*	PlayerBullet.h

*	���@�̒e

*/
class PlayerBullet:public IwasiEngine::Object3d
{
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
	static std::unique_ptr<PlayerBullet> Create
	(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model = nullptr,
		Particle* particle = nullptr, ParticleManager* pm = nullptr);
	//������(�������W�A���x)
	bool Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity);
	//���Z�b�g����
	void Reset();

	//�X�V
	void Update()override;

	//�`��
	void Draw();

	//�p�[�e�B�N���`��
	void DrawParticle();
	
	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute)override;

private://�����o�ϐ�

	//���x
	XMFLOAT3 velocity_;
	//����<frm>
	const float lifeTime_ = 60.0f;
	//���S����
	float deathTimer_ = lifeTime_;
	//���S�t���O
	bool isDead_ = false;
	//���a
	float radius_ = 1.0f;

	//���@������炤
	//�p�[�e�B�N��
	Particle* particleFire_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	ParticleManager* pmFire_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }

private://�J�v�Z���������o�֐�
	//�p�[�e�B�N���X�V
	void UpdateParticle();

};