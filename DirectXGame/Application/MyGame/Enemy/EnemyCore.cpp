#include "EnemyCore.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;
using namespace IwasiEngine;
/*

*	EnemyCore.cpp

*	�{�X�G�̃R�A

*/
//�����o�֐��|�C���^�e�[�u���̎���
void (EnemyCore::* EnemyCore::updateTable_[])() =
{
	&EnemyCore::UpdateCore,
	&EnemyCore::UpdateBreakCore,
	&EnemyCore::UpdateLeave
};

EnemyCore::~EnemyCore() {

}

std::unique_ptr<EnemyCore> EnemyCore::Create(const Model* model, const Model* bullet,
	const Player* player, GamePlayScene* gamescene, [[maybe_unused]] unsigned short level)
{
	//�C���X�^���X����
	std::unique_ptr<EnemyCore> ins = std::make_unique<EnemyCore>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);			//�{��
	if (bullet) ins->modelBullet_ = bullet;		//�e
	//���@�̃Z�b�g
	if (player)ins->SetPlayer(player);
	//�Q�[���V�[���̃Z�b�g
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// ������
bool EnemyCore::Initialize() {

	//������
	if (!Object3d::Initialize()) return false;

	startCount_ = std::chrono::steady_clock::now();	//�J�n����
	nowCount_ = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount_;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	//�G�����Ŕ\�͍͂U���^
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);
	//�e��p�����[�^�ݒ�
	Parameter();

	//�p�[�e�B�N��
	//��
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_.get());

	//��
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_.get());

	return true;
}

//�p�����[�^
void EnemyCore::Parameter() {

	//�t�F�[�Y������
	phase_ = Phase::CoreStage1;
	//���C�t�����l
	const int32_t startLife = 5;
	life_ = startLife;
	//���]������
	isReverse_ = false;
	//�e�����l
	enum MinMax
	{
		MM_min = 0,		//�ŏ��l
		MM_max = 1,		//�ő�l
		MM_num = 2,		//�z��p
	};
	//�G�e�̔��ˊԊu�̓����_��
	const std::array<int, MM_num>randomMinMax = { 50,100 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);	//���˃^�C�}�[������

	//���˃^�C�}�[������
	fireTimer_ = fireInterval_;
	//���S�t���O
	isDead_ = false;


}

//���Z�b�g
void EnemyCore::Reset() { Parameter(); }//�e��p�����[�^����

//�X�V
void EnemyCore::Update(const bool isStart) {

	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		//���W���ړ�������
		//switch (phase_) {
		//case EnemyCore::Phase::CoreStage1:	//�s����
		//	UpdateCore();
		//	break;

		//case EnemyCore::Phase::CoreBreak:	//���j���o��
		//	UpdateBreakCore();

		//	break;
		//case EnemyCore::Phase::Leave:		//���j��
		//	UpdateLeave();
		//	break;
		//}
		(this->*updateTable_[static_cast<size_t>(phase_)])();

	}
	//���W��]��
	Trans();
	//�X�V
	camera_->Update();		//�J����
	Object3d::Update();		//3D�I�u�W�F�N�g
	collider_->Update();	//�R���C�_�[

	//�p�[�e�B�N���X�V
	pmFire_->Update();
	pmSmoke_->Update();
}

//�e����
void EnemyCore::Fire() {
	assert(player_);

	//�e�̑��x
	const float kBulletSpeed = 0.5f;
	XMFLOAT3 velocity = {};

	//���@�̃��[���h���W���擾
	player_->GetWorldPosition();
	//�G�̃��[���h���W���擾
	GetWorldPosition();
	//�G�����@�̍����x�N�g�������߂�
	velocity.x = player_->GetWorldPosition().x - GetWorldPosition().x;
	velocity.y = player_->GetWorldPosition().y - GetWorldPosition().y;
	velocity.z = player_->GetWorldPosition().z - GetWorldPosition().z;
	// �x�N�g���̐��K��
	float len;
	len = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
	if (len != 0)
	{
		velocity.x /= len;
		velocity.y /= len;
		velocity.z /= len;
	}

	// �x�N�g���̒����𑬂��ɍ��킹��

	velocity.z -= kBulletSpeed;

	//���W���R�s�[
	XMFLOAT3 pos = GetPosition();

	//�e�𐶐���������
	std::unique_ptr<EnemyBullet> newBullet;
	newBullet = EnemyBullet::Create(pos, velocity, modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//�e��o�^
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

//�`��
void EnemyCore::Draw() {

	//���f���̕`��
	if (phase_ != Phase::Leave)Object3d::Draw();
}

void EnemyCore::DrawParticle()
{
	//�p�[�e�B�N���X�V
	pmFire_->Draw();
	pmSmoke_->Draw();
}

void EnemyCore::UpdateCore()
{
	//���x
	float cameraMove = camera_->GetEye().x;

	//���x�v�Z�p
	const float calcVelocity = hit_ * 0.2f;

	//���x
	XMFLOAT3 velocity;

	const XMFLOAT3 velDefault = { 0.3f + calcVelocity, 0.0f, 0.0f };//�ʏ펞�X�s�[�h
	const XMFLOAT3 velReverse = { -0.3f - calcVelocity, 0.0f, 0.0f };//���]���X�s�[�h

	//�ړ�
	if (!isReverse_)velocity = velDefault;
	else velocity = velReverse;

	position_.x += velocity.x;
	position_.y += velocity.y;
	position_.z += velocity.z;

	//X�����w��̈ʒu�ɓ��B�����甽�]
	const float reversePosX = 65.0f;

	if (position_.x >= reversePosX) {
		//�����灩
		isReverse_ = true;
	}
	if (position_.x <= -reversePosX) {
		//�����灨
		isReverse_ = false;
	}

	//���˃^�C�}�[�J�E���g�_�E��
	fireTimer_--;
	//�w�莞�ԂɒB����
	if (fireTimer_ <= endFireTime_) {
		//�e����
		Fire();
		//���˃^�C�}�[������
		const int32_t minInterval_ = fireInterval_;
		const int32_t maxInterval_ = fireInterval_ * 2;
		fireTimer_ = MyMath::RandomMTInt(minInterval_, maxInterval_);
	}
	//���񂾂玩�@�̒e�݂�����
	if (life_ <= deathLife_) {
		//���݃|�W�V�������L�^(�x�W�F�Ȑ��p)
		nowPos_ = Object3d::GetPosition();
		//���C�t��0��
		life_ = deathLife_;

		//�x�W�F�Ȑ��̒l
		const XMFLOAT3 startBezier3Pos = nowPos_;
		const XMFLOAT3 point1Bezier3Pos = { MyMath::RandomMTFloat(-30.0f,30.0f) + cameraMove,40.0f,70.0f };
		const XMFLOAT3 point2Bezier3Pos = { MyMath::RandomMTFloat(-30.0f,30.0f) + cameraMove,25.0f,85.0f };
		const XMFLOAT3 endBezier3Pos = { MyMath::RandomMTFloat(-15.0f,15.0f),10.0f,100.0f };
		//����_
		start_ = startBezier3Pos;
		point1_ = point1Bezier3Pos;
		point2_ = point2Bezier3Pos;
		end_ = endBezier3Pos;

		startCount_ = std::chrono::steady_clock::now();	//�J�n����
		//�G�ɂԂ���
		phase_ = Phase::CoreBreak;
	}
}

void EnemyCore::UpdateBreakCore()
{
	//���̈ʒu�܂ŒB�����瑮�������@�̒e�ɐ؂�ւ���
	if (position_.z >= point2_.z)
	{
		collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
		collider_->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);
	}
	//����
	//���ݎ��Ԃ��擾����
	nowCount_ = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

	const float micro = 1'000'000.0f;//�}�C�N���b
	float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / micro;//�}�C�N���b��b�ɒP�ʕϊ�
	//0~1
	const float timeRateMax = 1.0f;
	timeRate_ = min(elapsed / maxTime_, timeRateMax);
	//�x�W�F�Ȑ��Ŕ��ł���
	position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);

}

//���E
void EnemyCore::UpdateLeave() {
	//�T�u���������S���������ɂ���(���S���o�̂���)
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	//���̒l�܂ŃJ�E���g���i�񂾂玀�S����
	deathTimer_++;
	if (deathTimer_ >= DEATH_TIME)
	{
		isDead_ = true;
	}
}

const XMFLOAT3 EnemyCore::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
{
	//�O�_�x�W�F�Ȑ��̎�
	//B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t)*t^2 * P2 + t^3 * P3 0<=t<=1

	XMFLOAT3 ans;
	ans.x = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.x + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.x + 3.0f * (1.0f - t) * t * t * p2.x + t * t * t * p3.x;
	ans.y = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.y + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.y + 3.0f * (1.0f - t) * t * t * p2.y + t * t * t * p3.y;
	ans.z = (1.0f - t) * (1.0f - t) * (1.0f - t) * p0.z + 3.0f * (1.0f - t) * (1.0f - t) * t *
		p1.z + 3.0f * (1.0f - t) * t * t * p2.z + t * t * t * p3.z;

	return ans;
}

void EnemyCore::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;

	//���v���Z�b�g
	const ParticleManager::Preset smoke =
	{
		particleSmoke_.get(),
		position_,
		{ 0.0f ,0.0f,25.0f },
		{ 4.0f,4.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		30,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};
	//�����v���Z�b�g
	const ParticleManager::Preset fire =
	{
		particleFire_.get(),
		position_,
		{ 0.0f ,0.0f,25.0f },
		{ 4.0f,4.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		30,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;	//�n�`�̏ꍇ�����N����Ȃ�
	else if (attribute == COLLISION_ATTR_PLAYERS)		//���@�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;		 //���@�{�̂ɐG��Ă������N����Ȃ�
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)		 //���@�̒e�̏ꍇ
		{
			if (life_ > deathLife_)//���C�t��0����Ȃ��ꍇ
			{
				//�p�[�e�B�N���Ńq�b�g���o
				pmSmoke_->ActiveZ(smoke);

				pmSmoke_->Update();

			}
			else//���C�t��0�̏ꍇ
			{
				//�p�[�e�B�N���Ńq�b�g���o
				pmFire_->ActiveZ(fire);

				pmFire_->Update();

			}
			//���C�t������
			life_--;
			//���������J�E���g
			hit_++;
		}
	}
	else if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			pmFire_->ActiveZ(fire);

			pmFire_->Update();

			phase_ = Phase::Leave;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}
