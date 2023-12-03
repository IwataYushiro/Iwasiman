#include "Enemy1.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"

using namespace DirectX;

/*

*	Enemy1.cpp

*	���̂܂ܕ���������G

*/

//�ÓI�����o�ϐ��̎���
CollisionManager* Enemy1::colManager_ = CollisionManager::GetInstance();

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
	if (model) ins->SetModel(model);
	if (bullet) ins->modelBullet_ = bullet;
	if (player)ins->SetPlayer(player);
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

// ������
bool Enemy1::Initialize(const int level) {

	if (!Object3d::Initialize()) return false;
	

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	InitSubATTR(level);
	Parameter();

	//�p�[�e�B�N��
	particleSmoke_ = Particle::LoadFromParticleTexture("particle1.png");
	pmSmoke_ = ParticleManager::Create();
	pmSmoke_->SetParticleModel(particleSmoke_.get());

	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetParticleModel(particleFire_.get());

	return true;
}
void Enemy1::InitSubATTR(const int level)
{
	if (level == ET_Normal)collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	else if (level == ET_Power)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);
	else if (level == ET_Guard)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_GUARD);
	else if (level == ET_Speed)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_SPEED);
	else if (level == ET_Death)collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);
}

void Enemy1::InitSpeed()
{
	struct SpeedType
	{
		const XMFLOAT3 none = { -0.2f,0.0f,0.0f }; 
		const XMFLOAT3 power = { -0.1f,0.0f,0.0f };
		const XMFLOAT3 guard = { -0.2f,0.0f,0.0f };
		const XMFLOAT3 speed = { -0.4f,0.0f,0.0f };
		const XMFLOAT3 death = { -0.2f,0.0f,0.0f };
	};
	SpeedType speedType;
	
	//�ړ�
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) speed_ = speedType.none;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) speed_ = speedType.power;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) speed_ = speedType.guard;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) speed_ = speedType.speed;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) speed_ = speedType.death;

}

void Enemy1::InitLife()
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
void Enemy1::Parameter() {

	isReverse_ = false;
	//�W�����v������
	onGround_ = true;
	//�����t�F�[�Y
	phase_ = Phase::Approach;
	enum MinMax
	{
		MM_min = 0,
		MM_max = 1,
		MM_num = 2,
	};
	const std::array<int, MM_num>randomMinMax = { 100,150 };
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
void Enemy1::Reset() { Parameter(); }

//�X�V
void Enemy1::Update(const bool isStart) {

	pmFire_->SetCamera(camera_);
	pmSmoke_->SetCamera(camera_);
	if (!isStart)
	{
		//���W���ړ�������
		switch (phase_) {
		case Enemy1::Phase::Approach:
			UpdateApproach();
			break;
		case Enemy1::Phase::Leave:
			UpdateLeave();
			break;
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
void Enemy1::Trans() {

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
	
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
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
	if (phase_ != Phase::Leave)Object3d::Draw();


}

void Enemy1::DrawParticle()
{
	pmSmoke_->Draw();
	pmFire_->Draw();
}


//��ԕω��p�̍X�V�֐�
//�ڋ�
void Enemy1::UpdateApproach() {
	
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

	if (!onGround_)
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
		life_ = deathLife_;
		phase_ = Phase::Leave;
		
	}
	//Y���������W�ɒB���Ă����S
	const float deathFallPosY = -60.0f;
	if (position_.y <= deathFallPosY)isDead_ = true;
}

//���E
void Enemy1::UpdateLeave() {
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_ISDEAD);
	deathTimer_++;
	if (deathTimer_ >= DEATH_TIME)isDead_ = true;
}

//���[���h���W���擾
const XMFLOAT3 Enemy1::GetWorldPosition() const{

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void Enemy1::OnCollision([[maybe_unused]]const CollisionInfo& info,
	const unsigned short attribute,const unsigned short subAttribute)
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
		if (phase_ == Phase::Leave) return;
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
