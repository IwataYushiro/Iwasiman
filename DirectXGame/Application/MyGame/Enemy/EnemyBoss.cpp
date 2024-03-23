#include "EnemyBoss.h"
#include <cassert>
#include "SphereCollider.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	EnemyBoss.cpp

*	���E�ɓ����{�X�G

*/
//�����o�֐��|�C���^�e�[�u���̎���
void (EnemyBoss::* EnemyBoss::updateTable_[])() =
{
	&EnemyBoss::UpdateApproach,
	&EnemyBoss::UpdateAttack,
	&EnemyBoss::UpdateLeave
};

EnemyBoss::~EnemyBoss() {

}

std::unique_ptr<EnemyBoss> EnemyBoss::Create(const Model* model, const Model* bullet,
	const Player* player, GamePlayScene* gamescene, const bool isNotBossStage)
{
	//�C���X�^���X����
	std::unique_ptr<EnemyBoss> ins = std::make_unique<EnemyBoss>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);		//�{��
	if (bullet) ins->modelBullet_ = bullet;	//�e
	//���@�̃Z�b�g
	if (player)ins->SetPlayer(player);
	//�Q�[���V�[���̃Z�b�g
	if (gamescene)ins->SetGameScene(gamescene);
	//�{�X�X�e�[�W����Ȃ��ꍇ�J�����̓����ƘA��������
	if (isNotBossStage)ins->collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_NOTSTAGEBOSS);
	else ins->collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	return ins;
}

// ������
bool EnemyBoss::Initialize() {

	if (!Object3d::Initialize()) return false;
	//���͏����擾
	input_ = Input::GetInstance();

	//�x�W�F�Ȑ����
	startCount_ = std::chrono::steady_clock::now();	//�J�n����
	nowCount_ = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount_;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	//�G�{��
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

	//�e��p�����[�^�ݒ�
	Parameter();

	//�p�[�e�B�N��
	//��
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmSmoke_->SetParticleModel(particleSmoke_.get());

	//��
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmFire_->SetParticleModel(particleFire_.get());

	return true;
}

//�p�����[�^
void EnemyBoss::Parameter() {

	//�����t�F�[�Y
	phase_ = Phase::ApproachStage1;
	//���C�t�����l
	const int32_t startLife = 2;
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
	const std::array<int, MM_num>randomMinMax = { 40,75 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);
	//���˃^�C�}�[������
	fireTimer_ = fireInterval_;
	//���S�t���O
	isDead_ = false;
	bossDead_ = false;

}

//���Z�b�g
void EnemyBoss::Reset() { Parameter(); }//�e��p�����[�^����

//�X�V
void EnemyBoss::Update(const bool isStart) {

	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		//���W���ړ�������
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
void EnemyBoss::Fire() {
	assert(player_);

	//�e�̑��x
	const float kBulletSpeed = 0.8f / life_;
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
void EnemyBoss::Draw() {
	//���f���̕`��
	if (phase_ != Phase::Leave)Object3d::Draw();
}

void EnemyBoss::DrawParticle()
{
	//�e�p�[�e�B�N���`��
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//��ԕω��p�̍X�V�֐�
//�ڋ�
void EnemyBoss::UpdateApproach() {


	//�ʏ�X�e�[�W�̏ꍇ���@�ƃV���N��
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_NOTSTAGEBOSS)
	{
		UpdateSynchronizePlayerMove(true);
	}
	else UpdateSynchronizePlayerMove(false);//�{�X�X�e�[�W�̏ꍇ�V���N�����Ȃ�

	//���˃^�C�}�[�J�E���g�_�E��
	fireTimer_--;
	//�w�莞�ԂɒB����
	if (fireTimer_ <= endFireTime_) {
		//�e����
		Fire();
		//���˃^�C�}�[������
		const int32_t minInterval_ = fireInterval_ / 2;
		const int32_t maxInterval_ = fireInterval_;
		fireTimer_ = MyMath::RandomMTInt(minInterval_ * life_, maxInterval_ * life_);
	}
	//�����܂ŗ�����U���t�F�[�Y��
	const float moveAttackPhasePosZ = 100.0f;
	//�w��̈ʒu�ɓ��B������U��
	if (position_.z < moveAttackPhasePosZ) {

		//�x�W�F�Ȑ��̒l
		const XMFLOAT3 startBezier3Pos = { position_.x ,10.0f,moveAttackPhasePosZ };
		const XMFLOAT3 point1Bezier3Pos = { -5.0f,-20.0f,moveAttackPhasePosZ };
		const XMFLOAT3 point2Bezier3Pos = { 5.0f ,40.0f,moveAttackPhasePosZ };
		const XMFLOAT3 endBezier3Pos = { 15.0f ,10.0f,moveAttackPhasePosZ };

		//����_
		start_ = startBezier3Pos;
		point1_ = point1Bezier3Pos;
		point2_ = point2Bezier3Pos;
		end_ = endBezier3Pos;

		phase_ = Phase::AttackStage1;
	}
}
//�U��
void EnemyBoss::UpdateAttack() {

	//�ʏ�X�e�[�W�̏ꍇ���@�ƃV���N��
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_NOTSTAGEBOSS)
	{
		UpdateBezierMove(true);
	}
	else UpdateBezierMove(false);//�{�X�X�e�[�W�̏ꍇ�V���N�����Ȃ�
	

	//���˃^�C�}�[�J�E���g�_�E��
	fireTimer_--;
	//�w�莞�ԂɒB����
	if (fireTimer_ <= endFireTime_) {
		//�e����
		Fire();
		//���˃^�C�}�[������
		const int32_t minInterval_ = fireInterval_/2;
		const int32_t maxInterval_ = fireInterval_;
		fireTimer_ = MyMath::RandomMTInt(minInterval_, maxInterval_);
	}
	//���񂾂玀�S���o
	if (life_ <= deathLife_) {
		phase_ = Phase::Leave;
	}

}

//���E
void EnemyBoss::UpdateLeave() {

	//�T�u���������S���������ɂ���(���S���o�̂���)
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	//���̒l�܂ŃJ�E���g���i�񂾂玀�S����
	deathTimer_++;
	if (deathTimer_ >= DEATH_TIME)
	{
		isDead_ = true;
		bossDead_ = true;
	}
}

const XMFLOAT3 EnemyBoss::Bezier3(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, const float t)
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

void EnemyBoss::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;//���S���͉����N����Ȃ�

	//���݃��C�t�ɂ�锻�菈���̊�ƂȂ郉�C�t
	const int hitLife = 1;
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

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;	// �n�`�̏ꍇ�����N����Ȃ�
	else if (attribute == COLLISION_ATTR_PLAYERS)		// ���@�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;	   //���@�{�̂ɐG��Ă������N����Ȃ�
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)	   //���@�̒e�̏ꍇ
		{
			if (life_ > hitLife)//���C�t��1���傫���ꍇ
			{
				//�p�[�e�B�N���Ńq�b�g���o
				pmSmoke_->ActiveZ(smoke);

				pmSmoke_->Update();

			}
			else//1�ȉ��̏ꍇ
			{
				//�p�[�e�B�N���Ńq�b�g���o
				pmFire_->ActiveZ(fire);

				pmFire_->Update();
			}
			//���C�t������
			life_--;
		}

	}

}

void EnemyBoss::UpdateBezierMove(const bool notStageBoss)
{
	if (notStageBoss)//�ʏ�X�e�[�W�̏ꍇ�͏㉺�ړ�
	{
		//�_�b�V��
		if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
		{
			//����A�A�E��D
			if (input_->PushKey(DIK_A))position_.x -= player_->GetSpeedDash();
			else if (input_->PushKey(DIK_D))position_.x += player_->GetSpeedDash();
		}
		else//�ʏ�ړ�
		{
			//����A�A�E��D
			if (input_->PushKey(DIK_A))position_.x -= player_->GetSpeedMove();
			else if (input_->PushKey(DIK_D))position_.x += player_->GetSpeedMove();
		}

		//���x
		XMFLOAT3 velocity;
		//�o�ꑬ�x
		const XMFLOAT3 approachSpeed = { 0.0f,0.5f,0.0f };
		//�ړ�
		velocity = approachSpeed;
		position_.x += velocity.x;
		if(isReverse_)position_.y -= velocity.y;
		else position_.y += velocity.y;
		position_.z += velocity.z;
		//���̈ʒu�ɒB�����甽�](�㉺)
		const XMFLOAT2 reverseUpDownPosY = { 30.0f,0.0f };
		//�w��̈ʒu�ɓ��B�����甽�]
		if (position_.y >= reverseUpDownPosY.x) isReverse_ = true;	//�ォ�牺��
		if (position_.y <= reverseUpDownPosY.y) isReverse_ = false;	//��������
	}
	else//�x�W�F�Ȑ��̓{�X��p��
	{
		//�x�W�F�Ȑ��̒l
		const XMFLOAT3 startBezier3Pos = { -15.0f,10.0f,100.0f };
		const XMFLOAT3 point1Bezier3Pos = { -5.0f,-20.0f,100.0f };
		const XMFLOAT3 point2Bezier3Pos = { 5.0f,40.0f,100.0f };
		const XMFLOAT3 endBezier3Pos = { 15.0f,10.0f,100.0f };
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

		if (isReverse_)	//���]��
		{
			position_ = Bezier3(end_, point2_, point1_, start_, timeRate_);
		}
		else			//�ʏ펞
		{
			position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);
		}

		//�w��̈ʒu�ɓ��B�����甽�]
		if (position_.x >= end_.x) {
			//����_
			start_ = startBezier3Pos;
			point1_ = point1Bezier3Pos;
			point2_ = point2Bezier3Pos;
			end_ = endBezier3Pos;

			isReverse_ = true;
			startCount_ = std::chrono::steady_clock::now();
		}
		if (position_.x <= start_.x) {
			//����_
			start_ = startBezier3Pos;
			point1_ = point1Bezier3Pos;
			point2_ = point2Bezier3Pos;
			end_ = endBezier3Pos;

			isReverse_ = false;
			startCount_ = std::chrono::steady_clock::now();
		}
	}
}

void EnemyBoss::UpdateSynchronizePlayerMove(const bool notStageBoss)
{
	//���x
	XMFLOAT3 velocity;

	//�o�ꑬ�x
	const XMFLOAT3 approachSpeed = { 0.0f,0.0f,-0.5f };
	//�ړ�
	velocity = approachSpeed;
	if (notStageBoss)//�ʏ�X�e�[�W�̏ꍇ
	{
		//�_�b�V��
		if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
		{
			//����A�A�E��D
			if (input_->PushKey(DIK_A))velocity.x -= player_->GetSpeedDash();
			else if (input_->PushKey(DIK_D))velocity.x += player_->GetSpeedDash();
		}
		else//�ʏ�ړ�
		{
			//����A�A�E��D
			if (input_->PushKey(DIK_A))velocity.x -= player_->GetSpeedMove();
			else if (input_->PushKey(DIK_D))velocity.x += player_->GetSpeedMove();
		}
		position_.x += velocity.x;
		position_.y += velocity.y;
		position_.z += velocity.z;
	}
	else//�{�X�X�e�[�W�̏ꍇ�V���N�����Ȃ�
	{
		//�ړ�
		position_.x += velocity.x;
		position_.y += velocity.y;
		position_.z += velocity.z;
	}
}
