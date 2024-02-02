#include "Enemy1.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"

using namespace DirectX;
using namespace IwasiEngine;

/*

*	Enemy1.cpp

*	���̂܂ܕ���������G

*/

//�ÓI�����o�ϐ��̎���
IwasiEngine::CollisionManager* Enemy1::colManager_ = CollisionManager::GetInstance();
//�����o�֐��|�C���^�e�[�u���̎���
void (Enemy1::* Enemy1::updateTable_[])() =
{
	&Enemy1::UpdateApproach,
	&Enemy1::UpdateLeave
};

Enemy1::~Enemy1() {
	
}

std::unique_ptr<Enemy1> Enemy1::Create(const Model* model, const Model* bullet,
	const Player* player,GamePlayScene* gamescene, int level)
{
	//�C���X�^���X����
	std::unique_ptr<Enemy1> ins = std::make_unique<Enemy1>();
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
bool Enemy1::Initialize(const int level) {
	
	//������
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
void Enemy1::InitSubATTR(const int level)
{
	if (level == ET_Normal)collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);				//�ʏ푮��
	else if (level == ET_Power)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);	//�U������
	else if (level == ET_Guard)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_GUARD);	//�h�䑮��
	else if (level == ET_Speed)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_SPEED);	//���x����
	else if (level == ET_Death)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);	//�댯����
}

void Enemy1::InitSpeed()
{
	//�����ɂ���đ��x���Ⴄ
	struct SpeedType
	{
		const XMFLOAT3 none = { -0.2f,0.0f,0.0f }; 		  //�ʏ푮��
		const XMFLOAT3 power = { -0.1f,0.0f,0.0f };		  //�U������
		const XMFLOAT3 guard = { -0.2f,0.0f,0.0f };		  //�h�䑮��
		const XMFLOAT3 speed = { -0.4f,0.0f,0.0f };		  //���x����
		const XMFLOAT3 death = { -0.2f,0.0f,0.0f };		  //�댯����
	};
	SpeedType speedType;
	
	//�ړ�
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) speed_ = speedType.none;				//�ʏ푮��
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) speed_ = speedType.power;	//�U������
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) speed_ = speedType.guard;	//�h�䑮��
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) speed_ = speedType.speed;	//���x����
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) speed_ = speedType.death;	//�댯����

}

void Enemy1::InitLife()
{
	//�����ɂ���ă��C�t���Ⴄ
	struct LifeType
	{
		const int32_t none = 3;		  //�ʏ푮��
		const int32_t power = 3;	  //�U������
		const int32_t guard = 5;	  //�h�䑮��
		const int32_t speed = 2;	  //���x����
		const int32_t death = 2;	  //�댯����
	};
	LifeType lifeType;
	//���C�t
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) life_ = lifeType.none;				  //�ʏ푮��
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) life_ = lifeType.power;	  //�U������
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) life_ = lifeType.guard;	  //�h�䑮��
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) life_ = lifeType.speed;	  //���x����
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) life_ = lifeType.death;	  //�댯����
}

//�p�����[�^
void Enemy1::Parameter() {

	//���]������
	isReverse_ = false;
	//�W�����v������
	onGround_ = true;
	//�����t�F�[�Y
	phase_ = Phase::Approach;
	//�e�����l
	enum MinMax
	{
		MM_min = 0,		//�ŏ��l
		MM_max = 1,		//�ő�l
		MM_num = 2,		//�z��p
	};
	//�G�e�̔��ˊԊu�̓����_��
	const std::array<int, MM_num>randomMinMax = { 100,150 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);
	//���˃^�C�}�[������
	fireTimer_ = fireInterval_;

	//�ړ�
	InitSpeed();
	//���C�t
	InitLife();
	//���S�t���O
	isDead_ = false;
	
}

//���Z�b�g
void Enemy1::Reset() { Parameter(); }//�e��p�����[�^����

//�X�V
void Enemy1::Update(const bool isStart) {

	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);

	if (!isStart)//�X�^�[�g���o���͉������Ȃ�
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

	//���n����
	Landing();
	//�p�[�e�B�N���X�V
	pmFire_->Update();
	pmSmoke_->Update();
}

//�e����
void Enemy1::Fire() {
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
	newBullet = EnemyBullet::Create(pos, velocity,modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//�e��o�^
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

void Enemy1::Landing()
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
			float cos = XMVector3Dot(rejectDir, up).m128_f32[XYZ_X];

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
	ray.start = sphereCollider->center;
	ray.start.m128_f32[XYZ_Y] += sphereCollider->GetRadius();
	const XMVECTOR rayDir = { 0.0f,-1.0f,0.0f,0.0f };
	ray.dir = rayDir;
	//���C�L���X�g
	RaycastHit raycastHit;
	//���a�@X�@2.0f(radiusMulNum)
	const float radiusMulNum = 2.0f;
	//������ԂɂȂ�Y�l�̃X�s�[�h
	const float fallSpeedY = 0.0f;
	//�ڒn���
	if (onGround_)
	{
		//�X���[�Y�ɍ�����邽�߂̋z������
		const float adsDistance = 0.2f;
		//�ڒn���ێ�
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum + adsDistance))
		{
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);
			//�s��X�V
			Object3d::Update();
		}
		//�n�ʂ������̂ŗ���
		else
		{
			onGround_ = false;
			fallVec_ = {};
		}
	}
	//�������
	else if (fallVec_.y <= fallSpeedY)
	{
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum))
		{
			//���n
			onGround_ = true;
			position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * radiusMulNum);
			//�s��X�V
			Object3d::Update();
		}
	}

	//�s��X�V��
	Object3d::Update();
}

//�`��
void Enemy1::Draw() {

	//���f���̕`��
	Object3d::Draw();


}

void Enemy1::DrawParticle()
{
	//�e�p�[�e�B�N���`��
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//��ԕω��p�̍X�V�֐�
//�ڋ�
void Enemy1::UpdateApproach() {
	//���@�̋����[�G�̋��������̋�����菬���������瓮���悤�ɂ���
	const float updatePositionX = 90.0f;
	if (player_->GetPosition().x - position_.x <= -updatePositionX)return;

	//�ړ�
	position_.x += speed_.x;
	position_.y += speed_.y;
	position_.z += speed_.z;

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

	if (!onGround_)//�n�ʂ��痣�ꂽ�痎��
	{
		//�����������x
		const float fallAcc = -0.1f;
		const float fallVYMin = -2.0f;
		//����
		fallVec_.y = max(fallVec_.y + fallAcc, fallVYMin);
		//�ړ�
		position_.x += fallVec_.x;
		position_.y += fallVec_.y;
		position_.z += fallVec_.z;
	}

	//���񂾂�
	if (life_ <= deathLife_) {
		
		life_ = deathLife_;//���C�t���[����

		//�ŐV�̏����Z�b�g���Ď��S���o�̏���
		EaseDeadDirectionRotStart(false);	//��]
		EaseDeadDirectionScaleStart(false);	//�X�P�[��
		
		//�t�F�[�Y�؂�ւ�
		phase_ = Phase::Leave;
		
	}
	//Y���������W�ɒB���Ă����S
	const float deathFallPosY = -60.0f;
	if (position_.y <= deathFallPosY)isDead_ = true;
}

//���E
void Enemy1::UpdateLeave() {
	//�T�u���������S���������ɂ���(���S���o�̂���)
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	
	//�C�[�W���O�����]��
	EaseDeadDirectionRotStart(true);	//��]
	EaseDeadDirectionScaleStart(true);	//�X�P�[��
	//���̒l�܂ŃJ�E���g���i�񂾂玀�S����
	deathTimer_++;
	if (deathTimer_ >= DEATH_TIME)isDead_ = true;
}
void Enemy1::OnCollision([[maybe_unused]]const CollisionInfo& info,
	const unsigned short attribute,const unsigned short subAttribute)
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
		20,
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
		40,
		{ 3.0f, 0.0f },
		{ 1.0f,1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }
	};

	if (attribute == COLLISION_ATTR_LANDSHAPE)return;	//�n�`�̏ꍇ�����N����Ȃ�
	else if (attribute == COLLISION_ATTR_PLAYERS)		//���@�̏ꍇ
	{
		if (phase_ == Phase::Leave) return;						//���S���͉����N����Ȃ�
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;		//���@�{�̂ɐG��Ă������N����Ȃ�
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)		//���@�̒e�̏ꍇ
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
