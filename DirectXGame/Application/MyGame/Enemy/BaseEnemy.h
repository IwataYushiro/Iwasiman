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
	virtual bool Initialize([[maybe_unused]] unsigned short subAttribute) { return Object3d::Initialize(); }

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
	//�������W
	XMFLOAT3 startPos_;
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
		EL_Level0 = 0,
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
	const float DEATH_TIME = 70.0f;
	//���S�����̃C�[�W���O(Y��]��)
	const float END_ROTATION_Y = 360.0f * 3.0f;//3�T���
	const Easing presetEaseDeadDirectionRot_ = { 1.0f,0.0f,DEATH_TIME / 60.0f };//�C�[�W���O�̃v���Z�b�g(�Z�b�g�͑O��Ƃ���)
	Easing easeDeadDirectionRot_ = { presetEaseDeadDirectionRot_.start, presetEaseDeadDirectionRot_.end, presetEaseDeadDirectionRot_.maxtime };

	//���S�����̃C�[�W���O(�X�P�[��)
	//�C�[�W���O�̃v���Z�b�g(�Z�b�g�͑O��Ƃ���)
	const Easing presetEaseDeadDirectionScale_[XYZ_Num] =
	{
		{1.0f,0.0f,0.4f},		//X
		{1.0f,0.0f,0.4f},		//Y
		{1.0f,0.0f,0.4f},		//Z
	};
	//�X�P�[���͏��X�ɏ�����
	Easing easeDeadDirectionScale_[XYZ_Num] =
	{
		{ presetEaseDeadDirectionScale_[XYZ_X].start, presetEaseDeadDirectionScale_[XYZ_X].end, presetEaseDeadDirectionScale_[XYZ_X].maxtime },	//X
		{ presetEaseDeadDirectionScale_[XYZ_Y].start, presetEaseDeadDirectionScale_[XYZ_Y].end, presetEaseDeadDirectionScale_[XYZ_Y].maxtime },	//Y
		{ presetEaseDeadDirectionScale_[XYZ_Z].start, presetEaseDeadDirectionScale_[XYZ_Z].end, presetEaseDeadDirectionScale_[XYZ_Z].maxtime }	//Z
	};

public://�A�N�Z�b�T
	//���񂾂��ǂ���
	virtual bool IsDead() const { return isDead_; }
	//�{�X�����񂾂��ǂ���
	virtual bool BossDead()const { return bossDead_; }
	//�������W�̃Z�b�g
	virtual void SetStartPos(const XMFLOAT3& startpos) { startPos_ = startpos; }

protected://���L�����o�֐�
	//��]�C�[�W���O�̋��ʉ�(false = �X�^���o�C�Atrue = �A�N�e�B�u)
	void EaseDeadDirectionRotStart(const bool easeStart)
	{
		if (easeStart == false)//�X�^���o�C��
		{
			//�ŐV�̏����Z�b�g���Ď��S���o�̏���
			easeDeadDirectionRot_.SetEasing(rotation_.y, rotation_.y + END_ROTATION_Y, presetEaseDeadDirectionRot_.maxtime);
			easeDeadDirectionRot_.Standby(false);
		}
		else//�A�N�e�B�u��
		{
			//�C�[�W���O������]����]��
			easeDeadDirectionRot_.ease_in_out_sine();
			rotation_.y = easeDeadDirectionRot_.num_X;
		}
	}
	//�X�P�[���C�[�W���O�̋��ʉ�(false = �X�^���o�C�Atrue = �A�N�e�B�u)
	void EaseDeadDirectionScaleStart(const bool easeStart)
	{
		if (easeStart == false)//�X�^���o�C��
		{
			//�ŐV�̏����Z�b�g
			easeDeadDirectionScale_[XYZ_X].SetEasing(scale_.x, 0.0f, presetEaseDeadDirectionScale_[XYZ_X].maxtime);
			easeDeadDirectionScale_[XYZ_Y].SetEasing(scale_.y, 0.0f, presetEaseDeadDirectionScale_[XYZ_Y].maxtime);
			easeDeadDirectionScale_[XYZ_Z].SetEasing(scale_.z, 0.0f, presetEaseDeadDirectionScale_[XYZ_Z].maxtime);
			//���S���o�̏���
			for (uint32_t i = 0; i < XYZ_Num; i++)
			{
				easeDeadDirectionScale_[i].Standby(false);
			}

		}
		else//�A�N�e�B�u��
		{
			//�C�[�W���O�����X�P�[����]��
			for (uint32_t i = 0; i < XYZ_Num; i++)easeDeadDirectionScale_[i].ease_in_back();
			scale_.x = easeDeadDirectionScale_[XYZ_X].num_X;
			scale_.y = easeDeadDirectionScale_[XYZ_Y].num_X;
			scale_.z = easeDeadDirectionScale_[XYZ_Z].num_X;
			//�C�[�W���O���I������玀�S
			if (easeDeadDirectionScale_[XYZ_X].num_X == easeDeadDirectionScale_[XYZ_X].end)isDead_ = true;
		}
	}
};

