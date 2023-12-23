#pragma once
#include "Object3d.h"
#include "Easing.h"
#include "EnemyBullet.h"
#include "ParticleManager.h"
#include "CollisionManager.h"
#include "XYZ.h"
#include <DirectXMath.h>

//�O���錾
//���@�N���X
class Player;
//�Q�[���v���C�V�[��
class GamePlayScene;

/*

*	BaseEnemy.h (cpp X)

*	�G��ՃN���X

*/
class BaseEnemy :public IwasiEngine::Object3d
{
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::���ȗ�
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using Easing = IwasiEngine::Easing;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;
	using CollisionManager = IwasiEngine::CollisionManager;
	using CollisionInfo = IwasiEngine::CollisionInfo;
public:
	//�f�X�g���N�^
	virtual ~BaseEnemy() = default;

	//������
	virtual bool Initialize() { return Object3d::Initialize(); }
	//������(�T�u�����w���)
	virtual bool Initialize([[maybe_unused]] unsigned short subAttribute ) { return Object3d::Initialize(); }

	//�X�V
	virtual void Update(const bool isStart = false) = 0;
	//�`��
	virtual void Draw() { Object3d::Draw(); }
	//�p�[�e�B�N���`��
	virtual void DrawParticle() = 0;

protected:

	//���@
	const Player* player_ = nullptr;

	//�Q�[���v���C�V�[��
	GamePlayScene* gameScene_ = nullptr;

	//���S�t���O
	bool isDead_;
	//���C�t
	int life_;
	//���x
	XMFLOAT3 speed_;

	//�{�X�̎��S�t���O
	bool bossDead_;

	//�G����
	enum EnemyType
	{
		ET_Normal = 1,	//�ʏ�
		ET_Power = 2,	//�U���^
		ET_Guard = 3,	//�h��^
		ET_Speed = 4,	//���x�^
		ET_Death = 5,	//�댯�^
	};
	//�G���x��
	enum EnemyLevel
	{
		EL_Level0=0,
		EL_Level1 = 1,
		EL_Level2 = 2,
		EL_Level3 = 3,
		EL_Level4 = 4,
	};
	//����
	const int endFireTime_ = 0;
	//���C�t0�ȉ��Ŏ��S
	const int deathLife_ = 0;
	
	//�e���ˎ��Ԃ̓����_��

	//���S����
	int32_t deathTimer_ = 0;
	//�^�C�}�[�����̈ʒu�ɒB�����玀�S
	const int32_t DEATH_TIME = 70;

public://�A�N�Z�b�T
	//���񂾂��ǂ���
	virtual bool IsDead() const { return isDead_; }
	//�{�X�����񂾂��ǂ���
	virtual bool BossDead()const { return bossDead_; }
};

