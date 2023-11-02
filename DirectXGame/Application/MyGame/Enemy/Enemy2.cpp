#include "Enemy2.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
#include "Vector2.h"

using namespace DirectX;

/*

*	Enemy2.cpp

*	�����Ă͗�����G���G

*/

//�ÓI�����o�ϐ��̎���
CollisionManager* Enemy2::colManager_ = CollisionManager::GetInstance();

Enemy2::~Enemy2() {
	//�p�[�e�B�N�����f���̉��
	delete particleSmoke_;
	delete pmSmoke_;

	delete particleFire_;
	delete pmFire_;
}

std::unique_ptr<Enemy2> Enemy2::Create(Model* model, Model* bullet, Player* player, GamePlayScene* gamescene, int level)
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
	if (model) ins->SetModel(model);
	if (bullet) ins->modelBullet_ = bullet;
	if (player)ins->SetPlayer(player);
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// ������
bool Enemy2::Initialize(int level) {

	if (!Object3d::Initialize()) return false;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

	InitSubATTR(level);

	Parameter();

	//�p�[�e�B�N��
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_);

	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_);

	return true;
}

void Enemy2::InitSubATTR(int level)
{
	if (level == ET_Normal)collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	else if (level == ET_Power)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);
	else if (level == ET_Guard)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_GUARD);
	else if (level == ET_Speed)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_SPEED);
	else if (level == ET_Death)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);
}

void Enemy2::InitSpeed()
{
	//���x
	struct SpeedType
	{
		const XMFLOAT3 none = { 0.0f,-1.0f,0.0f };
		const XMFLOAT3 power = { 0.0f,-0.5f,0.0f };
		const XMFLOAT3 guard = { 0.0f,-1.0f,0.0f };
		const XMFLOAT3 speed = { 0.0f,-2.0f,0.0f };
		const XMFLOAT3 death = { 0.0f,-0.5f,0.0f };
	};
	SpeedType speedType;
	//�߂鑬�x
	struct BackSpeedType
	{
		const XMFLOAT3 none = { 0.0f,0.5f,0.0f };
		const XMFLOAT3 power = { 0.0f,0.25f,0.0f };
		const XMFLOAT3 guard = { 0.0f,0.5f,0.0f };
		const XMFLOAT3 speed = { 0.0f,1.0f,0.0f };
		const XMFLOAT3 death = { 0.0f,0.25f,0.0f };
	};
	BackSpeedType backSpeedType;
	//�ړ�
	//�ړ�
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE)
	{
		speed_ = speedType.none;
		backSpeed_ = backSpeedType.none;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER)
	{
		speed_ = speedType.power;
		backSpeed_ = backSpeedType.power;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD)
	{
		speed_ = speedType.guard;
		backSpeed_ = backSpeedType.guard;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED)
	{
		speed_ = speedType.speed;
		backSpeed_ = backSpeedType.speed;
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH)
	{
		speed_ = speedType.death;
		backSpeed_ = backSpeedType.death;
	}

}

void Enemy2::InitLife()
{
	struct LifeType
	{
		const int32_t none = 3;
		const int32_t power = 3;
		const int32_t guard = 5;
		const int32_t speed = 2;
		const int32_t death = 2;
	};
	LifeType lifeType;
	//���C�t
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) life_ = lifeType.none;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) life_ = lifeType.power;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) life_ = lifeType.guard;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) life_ = lifeType.speed;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) life_ = lifeType.death;
}

//�p�����[�^
void Enemy2::Parameter() {

	isReverse_ = false;
	//�W�����v������
	onGround_ = false;
	//�����t�F�[�Y
	phase_ = Phase::Approach;
	enum MinMax
	{
		MM_min = 0,
		MM_max=1,
		MM_num=2,
	};
	const std::array<int, MM_num>randomMinMax = { 75,100 };
	fireInterval_ = MyMath::RandomMTInt(randomMinMax[MM_min], randomMinMax[MM_max]);
	//���˃^�C�}�[������
	fireTimer_ = fireInterval_;

	//�ړ�
	InitSpeed();
	//���C�t
	InitLife();

	isDead_ = false;


}

//���Z�b�g
void Enemy2::Reset() { Parameter(); }

//�X�V
void Enemy2::Update(bool isStart) {

	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		//���W���ړ�������
		switch (phase_) {
		case Enemy2::Phase::Approach:

			UpdateApproach();
			break;
		case Enemy2::Phase::Back:
			UpdateBack();
			break;
		case Enemy2::Phase::Leave:
			UpdateLeave();
			break;
		}
		if (phase_ != Phase::Leave)
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

		//���񂾂�
		if (life_ <= deathLife_) {
			phase_ = Phase::Leave;
		}
	}

	//�s��X�V
	Trans();
	camera_->Update();
	Object3d::Update();
	collider_->Update();

	Landing();
	//�p�[�e�B�N���X�V
	pmFire_->Update();
	pmSmoke_->Update();
}

//�]��
void Enemy2::Trans() {

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

void Enemy2::Landing()
{
	//���R���C�_�[�̎擾
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider_);
	assert(sphereCollider);

	//��p�N�G���[�R�[���o�b�N�N���X��`
	class EnemyQueryCallback : public QueryCallback
	{
	public:
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

	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();

	eyepos.x += callback.move.m128_f32[XYZ_X];

	tarpos.x += callback.move.m128_f32[XYZ_X];

	//�R���C�_�[�X�V
	UpdateWorldMatrix();
	camera_->SetEye(eyepos);
	camera_->SetTarget(tarpos);
	collider_->Update();

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	const XMVECTOR rayDir = { 0.0f,-1.0f,0.0f,0.0f };
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = rayDir;
	RaycastHit raycastHit;
	//���a�@X�@2.0f(radiusMulNum)
	const float radiusMulNum = 2.0f;
	//�ڒn���
	if (onGround_)
	{
		//�ڒn���ێ�
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
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * radiusMulNum))
		{
			onGround_ = true;
		}

		if (position_.y >= backUpPosY)
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
	if (phase_ != Phase::Leave)Object3d::Draw();


}

void Enemy2::DrawParticle()
{
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//��ԕω��p�̍X�V�֐�
//�ڋ�
void Enemy2::UpdateApproach() {

	//�ړ�
	if (!onGround_)
	{
		position_.x += speed_.x;
		position_.y += speed_.y;
		position_.z += speed_.z;
	}

	if (position_.y <= backFallPosY)
	{

		phase_ = Phase::Leave;
	}
}

void Enemy2::UpdateBack()
{
	position_.x += backSpeed_.x;
	position_.y += backSpeed_.y;
	position_.z += backSpeed_.z;

	if (position_.y >= backUpPosY) phase_ = Phase::Approach;
}

//���E
void Enemy2::UpdateLeave() {
	deathTimer_++;

	if (deathTimer_ >= DEATH_TIME)isDead_ = true;
}

//���[���h���W���擾
XMFLOAT3 Enemy2::GetWorldPosition() {

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void Enemy2::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (phase_ == Phase::Leave)return;
	const int hitLife = deathLife_ + 1;
	//���v���Z�b�g
	const ParticleManager::Preset smoke =
	{
		particleSmoke_,
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
		particleFire_,
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
