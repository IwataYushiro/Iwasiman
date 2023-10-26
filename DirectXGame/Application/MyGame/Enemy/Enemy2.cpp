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
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, this->radius_));
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

	InitSubATTR(level);

	Parameter();

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
	//�ړ�
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE)
	{
		speed_ = { 0.0f, -1.0f, 0.0f };
		backSpeed_ = { 0.0f,0.5f,0.0f };
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER)
	{
		speed_ = { 0.0f, -0.5f, 0.0f };
		backSpeed_ = { 0.0f,0.25f,0.0f };
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD)
	{
		speed_ = { 0.0f, -1.0f, 0.0f };
		backSpeed_ = { 0.0f,0.5f,0.0f };
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED)
	{
		speed_ = { 0.0f, -2.0f, 0.0f };
		backSpeed_ = { 0.0f,1.0f,0.0f };
	}
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH)
	{
		speed_ = { 0.0f, -0.5f, 0.0f };
		backSpeed_ = { 0.0f,0.25f,0.0f };
	}

}

void Enemy2::InitLife()
{
	//���C�t
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) life_ = 3;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) life_ = 3;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) life_ = 5;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) life_ = 2;
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) life_ = 2;
}

//�p�����[�^
void Enemy2::Parameter() {

	isReverse_ = false;
	//�W�����v������
	onGround_ = false;
	//�����t�F�[�Y
	phase_ = Phase::Approach;
	const std::array<int, 2>RandomMinMax = { 75,100 };
	fireInterval_ = MyMath::RandomMTInt(RandomMinMax[0], RandomMinMax[1]);
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

	if (!isStart)
	{


		//���W���ړ�������
		switch (phase_) {
		case Enemy2::Phase::Approach:

			UpdateApproach();
			break;
		case Enemy2::Phase::Leave:
			UpdateLeave();
			break;
		}

		//���˃^�C�}�[�J�E���g�_�E��
		fireTimer_--;
		//�w�莞�ԂɒB����
		if (fireTimer_ <= endFireTime_) {
			//�e����
			Fire();
			//���˃^�C�}�[������
			const int minInterval = fireInterval_ / 2;
			fireTimer_ = MyMath::RandomMTInt(minInterval, fireInterval_);
		}


		//���񂾂�
		if (life_ <= deathLife_) {
			isDead_ = true;
			life_ = deathLife_;
		}
	}

	//�s��X�V
	Trans();
	camera_->Update();
	Object3d::Update();
	collider_->Update();

	Landing();
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
	position_.x += callback.move.m128_f32[0];
	position_.y += callback.move.m128_f32[1];
	//position_.z += callback.move.m128_f32[2];

	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();

	eyepos.x += callback.move.m128_f32[0];

	tarpos.x += callback.move.m128_f32[0];

	//�R���C�_�[�X�V
	UpdateWorldMatrix();
	camera_->SetEye(eyepos);
	camera_->SetTarget(tarpos);
	collider_->Update();

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RaycastHit raycastHit;
	//���a�@X�@2.0f(radiusMulNum)
	const float radiusMulNum = 2.0f;
	//�ڒn���
	if (onGround_)
	{
		//�ڒn���ێ�
		if (count_ == MAX_GROUND)
		{
			onGround_ = false;
			count_ = 0;
			upPos_ = Object3d::GetPosition();
			phase_ = Phase::Leave;
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
	Object3d::Draw();


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

	if (position_.y <= -backFallPosY)
	{

		phase_ = Phase::Leave;
	}
}

//���E
void Enemy2::UpdateLeave() {
	position_.x += backSpeed_.x;
	position_.y += backSpeed_.y;
	position_.z += backSpeed_.z;

	if (position_.y >= backUpPosY) phase_ = Phase::Approach;
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
	if (attribute == COLLISION_ATTR_LANDSHAPE)return;
	else if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_--;
	}

}
