#include "EnemyBoss.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"

using namespace DirectX;

/*

*	EnemyBoss.cpp

*	���E�ɓ����{�X�G

*/

//�ÓI�����o�ϐ��̎���
CollisionManager* EnemyBoss::colManager_ = CollisionManager::GetInstance();

EnemyBoss::~EnemyBoss() {
	
}

std::unique_ptr<EnemyBoss> EnemyBoss::Create(const Model* model, const Model* bullet,
	const Player* player, GamePlayScene* gamescene)
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
	if (model) ins->SetModel(model);
	if (bullet) ins->modelBullet_ = bullet;
	if (player)ins->SetPlayer(player);
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// ������
bool EnemyBoss::Initialize() {

	if (!Object3d::Initialize()) return false;

	startCount_ = std::chrono::steady_clock::now();	//�J�n����
	nowCount_ = std::chrono::steady_clock::now();		//���ݎ���
	elapsedCount_;	//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	Parameter();

	//�p�[�e�B�N��
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmSmoke_->SetParticleModel(particleSmoke_.get());

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

	const int32_t startLife = 2;
	life_ = startLife;

	isReverse_ = false;
	enum MinMax
	{
		MM_min = 0,
		MM_max=1,
		MM_num=2,
	};
	const std::array<int, MM_num>randomMinMax = { 40,75 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);

	//���˃^�C�}�[������
	fireTimer_ = fireInterval_;

	isDead_ = false;
	bossDead_ = false;

}

//���Z�b�g
void EnemyBoss::Reset() { }

//�X�V
void EnemyBoss::Update(const bool isStart) {

	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		//���W���ړ�������
		switch (phase_) {
		case EnemyBoss::Phase::ApproachStage1:

			UpdateApproach();
			break;

		case EnemyBoss::Phase::AttackStage1:

			UpdateAttack();

			break;

		case EnemyBoss::Phase::Leave:
			UpdateLeave();
			break;
		}
	}
	//�s��X�V
	Trans();
	camera_->Update();
	Object3d::Update();
	//�p�[�e�B�N���X�V
	pmFire_->Update();
	pmSmoke_->Update();
}

//�]��
void EnemyBoss::Trans() {

	XMMATRIX world;
	//�s��X�V
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(Object3d::GetScale().x, Object3d::GetScale().y, Object3d::GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(Object3d::GetRotation().z)
		* XMMatrixRotationX(Object3d::GetRotation().x) * XMMatrixRotationY(Object3d::GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(Object3d::GetPosition().x,
		Object3d::GetPosition().y, Object3d::GetPosition().z);

	//����
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	Object3d::SetWorld(world);

}
//�e����
void EnemyBoss::Fire() {
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
void EnemyBoss::Draw() {
	//���f���̕`��
	if (phase_ != Phase::Leave)Object3d::Draw();
}

void EnemyBoss::DrawParticle()
{
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//��ԕω��p�̍X�V�֐�
//�ڋ�
void EnemyBoss::UpdateApproach() {
	//���x
	XMFLOAT3 velocity;

	const XMFLOAT3 approachSpeed = { 0.0f,0.0f,-0.5f };
	//�ړ�
	velocity = approachSpeed;
	position_.x += velocity.x;
	position_.y += velocity.y;
	position_.z += velocity.z;

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
	//�����܂ŗ�����U���t�F�[�Y��
	const float moveAttackPhasePosZ = 100.0f;
	//�w��̈ʒu�ɓ��B������U��
	if (position_.z < moveAttackPhasePosZ) {

		//�x�W�F�Ȑ��̒l
		const XMFLOAT3 startBezier3Pos = { position_.x ,10.0f,moveAttackPhasePosZ };
		const XMFLOAT3 point1Bezier3Pos = { -10.0f ,-20.0f,moveAttackPhasePosZ };
		const XMFLOAT3 point2Bezier3Pos = { 10.0f ,40.0f,moveAttackPhasePosZ };
		const XMFLOAT3 endBezier3Pos = { 30.0f ,10.0f,moveAttackPhasePosZ };

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

	//����
	//���ݎ��Ԃ��擾����
	nowCount_ = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = std::chrono::duration_cast<std::chrono::microseconds>(nowCount_ - startCount_);

	const float micro = 1'000'000.0f;//�}�C�N���b
	float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsedCount_).count() / micro;//�}�C�N���b��b�ɒP�ʕϊ�

	const float timeRateMax = 1.0f;
	timeRate_ = min(elapsed / maxTime_, timeRateMax);

	if (isReverse_) {
		position_ = Bezier3(end_, point2_, point1_, start_, timeRate_);
	}
	else {
		position_ = Bezier3(start_, point1_, point2_, end_, timeRate_);
	}

	//�x�W�F�Ȑ��̒l
	const XMFLOAT3 startBezier3Pos = { -30.0f ,10.0f,100.0f };
	const XMFLOAT3 point1Bezier3Pos = { -10.0f ,-20.0f,100.0f };
	const XMFLOAT3 point2Bezier3Pos = { 10.0f ,40.0f,100.0f };
	const XMFLOAT3 endBezier3Pos = { 30.0f ,10.0f,100.0f };
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
	//���񂾂�
	if (life_ <= deathLife_) {
		phase_ = Phase::Leave;
	}

}

//���E
void EnemyBoss::UpdateLeave() {

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


//���[���h���W���擾
const XMFLOAT3 EnemyBoss::GetWorldPosition() const{

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void EnemyBoss::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;
	const int hitLife = deathLife_ + 1;
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

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;
	else if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)
		{
			if (life_ > hitLife)
			{

				pmSmoke_->ActiveZ(smoke.particle, smoke.startPos, smoke.pos, smoke.vel,
					smoke.acc, smoke.num, smoke.scale, smoke.startColor, smoke.endColor);

				pmSmoke_->Update();
				life_--;
			}
			else
			{

				pmFire_->ActiveZ(fire.particle, fire.startPos, fire.pos, fire.vel,
					fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);

				pmFire_->Update();
				life_--;
			}
		}

	}

}
