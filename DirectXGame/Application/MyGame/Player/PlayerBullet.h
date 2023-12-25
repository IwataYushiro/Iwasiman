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
	(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model = nullptr);
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

	//���[���h���W���擾
	const XMFLOAT3 GetWorldPosition()const;

private://�����o�ϐ�

	//���x
	XMFLOAT3 velocity_;
	//����<frm>
	static const int32_t lifeTime_ = 60 * 2;
	//���S����
	int32_t deathTimer_ = lifeTime_;
	//���S�t���O
	bool isDead_ = false;
	//���a
	float radius_ = 4.0f;

	//�p�[�e�B�N��
	std::unique_ptr<Particle> particleFire_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pmFire_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }

private://�J�v�Z���������o�֐�
	//�p�[�e�B�N���X�V
	void UpdateParticle();

};