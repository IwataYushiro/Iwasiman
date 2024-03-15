#include "Enemy2.h"
#include <cassert>
#include "SphereCollider.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
#include "Vector2.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	Enemy2.cpp

*	�����Ă͗�����G���G

*/

//�ÓI�����o�ϐ��̎���
IwasiEngine::CollisionManager* Enemy2::colManager_ = CollisionManager::GetInstance();
//�����o�֐��|�C���^�e�[�u���̎���
void (Enemy2::* Enemy2::updateTable_[])() =
{
	&Enemy2::UpdateApproach,
	&Enemy2::UpdateBack,
	&Enemy2::UpdateLeave
};

Enemy2::~Enemy2() {

}

std::unique_ptr<Enemy2> Enemy2::Create(const Model* model, const Model* bullet,
	const Player* player, GamePlayScene* gamescene, int level)
{
	//�C���X�^���X����
	std::unique_ptr<Enemy2> ins = std::make_unique<Enemy2>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize(level))
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
	return ins;
}

// ������
bool Enemy2::Initialize(const int level) {

	if (!Object3d::Initialize()) return false;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	//�G�{��
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	//�����ŃT�u������������
	InitSubATTR(level);
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

void Enemy2::InitSubATTR(const int level)
{
	if (level == ET_Normal)collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);				//�ʏ푮��
	else if (level == ET_Power)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);	//�U������
	else if (level == ET_Guard)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_GUARD);	//�h�䑮��
	else if (level == ET_Speed)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_SPEED);	//���x����
	else if (level == ET_Death)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);	//�댯����
}

void Enemy2::InitSpeed()
{
	//���x
	struct SpeedType
	{
		const XMFLOAT3 none = { 0.0f,-1.0f,0.0f };		  //�ʏ푮��
		const XMFLOAT3 power = { 0.0f,-0.5f,0.0f };		  //�U������
		const XMFLOAT3 guard = { 0.0f,-1.0f,0.0f };		  //�h�䑮��
		const XMFLOAT3 speed = { 0.0f,-2.0f,0.0f };		  //���x����
		const XMFLOAT3 death = { 0.0f,-0.5f,0.0f };		  //�댯����
	};
	SpeedType speedType;
	//�߂鑬�x
	struct BackSpeedType
	{
		const XMFLOAT3 none = { 0.0f,0.5f,0.0f };  		  //�ʏ푮��
		const XMFLOAT3 power = { 0.0f,0.25f,0.0f };		  //�U������
		const XMFLOAT3 guard = { 0.0f,0.5f,0.0f }; 		  //�h�䑮��
		const XMFLOAT3 speed = { 0.0f,1.0f,0.0f }; 		  //���x����
		const XMFLOAT3 death = { 0.0f,0.25f,0.0f };		  //�댯����
	};
	BackSpeedType backSpeedType;
	//�ړ�
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE)					//�ʏ푮��
	{
		speed_ = speedType.none;
		backSpeed_ = backSpeedType.none;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER)		//�U������
	{
		speed_ = speedType.power;
		backSpeed_ = backSpeedType.power;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD)		//�h�䑮��
	{
		speed_ = speedType.guard;
		backSpeed_ = backSpeedType.guard;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED)		//���x����
	{
		speed_ = speedType.speed;
		backSpeed_ = backSpeedType.speed;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH)		//�댯����
	{
		speed_ = speedType.death;
		backSpeed_ = backSpeedType.death;
	}

}

void Enemy2::InitLife()
{
	struct LifeType
	{
		const int32_t none = 3;		//�ʏ푮��
		const int32_t power = 3;	//�U������
		const int32_t guard = 5;	//�h�䑮��
		const int32_t speed = 2;	//���x����
		const int32_t death = 2;	//�댯����
	};
	LifeType lifeType;
	//���C�t
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) life_ = lifeType.none;				   //�ʏ푮��
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) life_ = lifeType.power;	   //�U������
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) life_ = lifeType.guard;	   //�h�䑮��
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) life_ = lifeType.speed;	   //���x����
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) life_ = lifeType.death;	   //�댯����
}

//�p�����[�^
void Enemy2::Parameter() {

	//���]������
	isReverse_ = false;
	//�W�����v������
	onGround_ = false;
	//�����t�F�[�Y
	phase_ = Phase::Approach;
	//�e�����l
	enum MinMax
	{
		MM_min = 0,
		MM_max = 1,
		MM_num = 2,
	};
	//�G�e�̔��ˊԊu�̓����_��
	const std::array<int, MM_num>randomMinMax = { 75,100 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);
	//���˃^�C�}�[������
	fireTimer_ = fireInterval_;

	//�ړ�
	InitSpeed();
	//���C�t
	InitLife();
	//�F
	InitColor();
	//���S�t���O
	isDead_ = false;


}

//���Z�b�g
void Enemy2::Reset() { Parameter(); }//�e��p�����[�^����

//�X�V
void Enemy2::Update(const bool isStart) {

	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);

	if (!isStart)//�X�^�[�g���o���͉������Ȃ�
	{
		//���W���ړ�������
		(this->*updateTable_[static_cast<size_t>(phase_)])();
		//���S����(Leave�����S���o)
		if (phase_ != Phase::Leave)Dead();

	}
	//���W�̓]��
	Trans();
	//�X�V
	camera_->Update();		//�J����
	Object3d::Update();		//3D�I�u�W�F�N�g
	collider_->Update();	//�R���C�_�[

	//���n����
	Landing();
	//�p�[�e�B�N���X�V
	pmFire_->Update();
	pmSmoke_->Update();
}
void Enemy2::Attack()
{
	//���˃^�C�}�[�J�E���g�_�E��
	fireTimer_--;
	//�w�莞�ԂɒB����
	if (fireTimer_ <= endFireTime_) {
		//�e����
		Fire();
		//���˃^�C�}�[������
		const int32_t minInterval = fireInterval_ / 2;
		const int32_t maxInterval = fireInterval_;
		fireTimer_ = MyMath::RandomMTInt(minInterval, maxInterval);
	}
}

//�e����
void Enemy2::Fire() {
	assert(player_);

	//�e�̑��x
	const float kBulletSpeed = 0.3f;
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

void Enemy2::Dead()
{
	//���C�t��0�ɂȂ�����
	if (life_ <= deathLife_) {

		life_ = deathLife_;//���C�t���[����

		//�ŐV�̏����Z�b�g���Ď��S���o�̏���
		EaseDeadDirectionRotStart(false);	//��]
		EaseDeadDirectionScaleStart(false);	//�X�P�[��
		//�t�F�[�Y�؂�ւ�
		phase_ = Phase::Leave;
	}

}

void Enemy2::Landing()
{
	//���R���C�_�[�̎擾
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider_);
	assert(sphereCollider);

	//��p�N�G���[�R�[���o�b�N�N���X��`
	class EnemyQueryCallback : public QueryCallback
	{
	public:
		//�R���X�g���N�^
		EnemyQueryCallback(Sphere* sphere) :sphere_(sphere) {};

		//�Փˎ��̃R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info)
		{
			//���[���h�����
			const XMVECTOR up = { 0.0f,1.0f,0.0f,0.0f };
			//�r�˕���
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//������Ɣr�˕����̊p�x���̃R�T�C���l
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			//�n�ʔ���̂������l�p�x
			const float threshold = cosf(XMConvertToRadians(30.0f));
			//�p�x���ɂ���ēV�䖔�͒n�ʂƔ��肳���ꍇ��������
			if (-threshold < cos && cos < threshold)
			{
				//����r��(�����o��)
				sphere_->center += info.reject;
				move += info.reject;
			}

			return true;
		}

	public:
		//��
		Sphere* sphere_ = nullptr;
		//�r�˂ɂ��ړ���
		XMVECTOR move = {};

	};

	// ���N�G���[�A�R���C�_�[�X�V
	//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
	EnemyQueryCallback callback(sphereCollider);

	//���ƒn�`�̌�����S����
	colManager_->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);
	//�����ɂ��r�˕�������
	position_.x += callback.move.m128_f32[XYZ_X];
	position_.y += callback.move.m128_f32[XYZ_Y];
	//position_.z += callback.move.m128_f32[2];

	//���_�ƒ����_���Q�b�g
	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();
	//X������������
	eyepos.x += callback.move.m128_f32[XYZ_X];
	tarpos.x += callback.move.m128_f32[XYZ_X];

	//�X�V
	UpdateWorldMatrix();			//�s��X�V
	camera_->SetEye(eyepos);		//���_�Z�b�g
	camera_->SetTarget(tarpos);		//�����_�Z�b�g
	collider_->Update();			//�R���C�_�[

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	const XMVECTOR rayDir = { 0.0f,-1.0f,0.0f,0.0f };
	ray.start = sphereCollider->center;
	ray.start.m128_f32[XYZ_Y] += sphereCollider->GetRadius();
	ray.dir = rayDir;
	RaycastHit raycastHit;
	//���a�@X�@2.0f(radiusMulNum)
	const float radiusMulNum = 2.0f;
	//�ڒn���
	if (onGround_)
	{
		//��ނ���
		if (count_ == MAX_GROUND)
		{
			const int8_t resetCount = 0;
			onGround_ = false;
			count_ = resetCount;
			upPos_ = Object3d::GetPosition();
			phase_ = Phase::Back;
		}

		count_++;
	}
	//�������
	else
	{
		//�ڒn
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum))
		{
			onGround_ = true;
		}
		//�����ʒu�ɒB������܂�������悤��
		if (position_.y >= startPos_.y)
		{
			phase_ = Phase::Approach;
		}
	}
	//�s��X�V��
	Object3d::Update();
}

//�`��
void Enemy2::Draw() {
	//���f���̕`��
	Object3d::Draw();
}

void Enemy2::DrawParticle()
{
	//�e�p�[�e�B�N���`��
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//��ԕω��p�̍X�V�֐�
//���~
void Enemy2::UpdateApproach() {

	const float updatePositionX = 90.0f;
	if (player_->GetPosition().x - position_.x <= -updatePositionX)return;
	//�ړ�
	if (!onGround_)
	{
		position_.x += speed_.x;
		position_.y += speed_.y;
		position_.z += speed_.z;
	}
	//�U������
	Attack();
	//���̈ʒu�܂ŒB��������
	if (position_.y <= backFallPosY)phase_ = Phase::Back;

}

//�㏸
void Enemy2::UpdateBack()
{
	const float updatePositionX = 90.0f;
	if (player_->GetPosition().x - position_.x <= -updatePositionX)return;
	//�ړ�
	position_.x += backSpeed_.x;
	position_.y += backSpeed_.y;
	position_.z += backSpeed_.z;
	//�U������
	Attack();
	//�����ʒu�܂ŒB�����牺��
	if (position_.y >= startPos_.y) phase_ = Phase::Approach;
}

//���E
void Enemy2::UpdateLeave() {

	//�T�u���������S���������ɂ���(���S���o�̂���)
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	//�C�[�W���O�����]��
	EaseDeadDirectionRotStart(true);	//��]
	EaseDeadDirectionScaleStart(true);	//�X�P�[��
	//���̒l�܂ŃJ�E���g���i�񂾂玀�S����
	deathTimer_++;
	if (deathTimer_ >= DEATH_TIME)isDead_ = true;
}

void Enemy2::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;//���S���͉����N����Ȃ�
	if (life_ <= 0) return;
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
		20,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f },
		20
	};
	//�����v���Z�b�g
	const ParticleManager::Preset fire =
	{
		particleFire_.get(),
		position_,
		{ 0.0f ,0.0f,25.0f },
		{ 4.0f,4.0f,0.0f },
		{ 0.0f,0.001f,0.0f },
		40,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f },
		20
	};

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;	  //�n�`�̏ꍇ�����N����Ȃ�
	else if (attribute == COLLISION_ATTR_PLAYERS)		  //���@�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;		 //���@�{�̂ɐG��Ă������N����Ȃ�
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)		 //���@�̒e�̏ꍇ
		{
			//���񂾂�X�L�b�v
			if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_ISDEAD) return;
			if (life_ > hitLife)//���C�t��1���傫���ꍇ
			{
				//�p�[�e�B�N���Ńq�b�g���o
				pmSmoke_->ActiveZ(smoke);

				pmSmoke_->Update();
				
			}
			else//1�ȉ��̏ꍇ
			{
				//�J���[�ύX
				color_ = hitColor_;
				//�p�[�e�B�N���Ńq�b�g���o
				pmFire_->ActiveZ(fire);

				pmFire_->Update();
			}

			//���C�t������
			life_--;
		}
	}

}
