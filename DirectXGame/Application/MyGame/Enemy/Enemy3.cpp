#include "Enemy3.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;

CollisionManager* Enemy3::colManager_ = CollisionManager::GetInstance();

Enemy3::~Enemy3() {
}

std::unique_ptr<Enemy3> Enemy3::Create(Model* model, Model* bullet, Player* player, GamePlayScene* gamescene, int level)
{
	//�C���X�^���X����
	std::unique_ptr<Enemy3> ins = std::make_unique<Enemy3>();
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
bool Enemy3::Initialize(int level) {

	if (!Object3d::Initialize()) return false;
	input_ = Input::GetInstance();

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);

	InitSubATTR(level);

	Parameter(level);

	return true;
}

void Enemy3::InitSubATTR(int level)
{
	if (level == 1)collider->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	else if (level == 2)collider->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_POWER);
	else if (level == 3)collider->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_GUARD);
	else if (level == 4)collider->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_SPEED);
	else if (level == 5)collider->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);
}

void Enemy3::InitSpeed()
{//�ړ�
	if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) speed = { 0.0f, 0.0f, -0.1f };
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) speed = { 0.0f,0.0f,-0.05f };
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) speed = { 0.0f,0.0f,-0.1f };
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) speed = { 0.0f,0.0f,-0.3f };
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) speed = { 0.0f,0.0f,-0.1f };

}

void Enemy3::InitLIfe()
{
	//���C�t
	if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_NONE) life_ = 3;
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_POWER) life_ = 3;
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_GUARD) life_ = 6;
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_SPEED) life_ = 2;
	else if (collider->GetSubAttribute() == SUBCOLLISION_ATTR_ENEMY_DEATH) life_ = 2;
}

//�p�����[�^
void Enemy3::Parameter(int level) {

	isReverse_ = false;
	//�W�����v������
	onGround = true;
	//�����t�F�[�Y
	phase_ = Phase::Approach;

	//���˃^�C�}�[������
	fireTimer = kFireInterval;

	//�ړ�
	InitSpeed();
	//���C�t
	InitLIfe();

	isDead_ = false;

	kFireInterval = MyMath::RandomMTInt(100, 150);
}

//���Z�b�g
void Enemy3::Reset(int level) { Parameter(level); }

//�X�V
void Enemy3::Update() {


	//���W���ړ�������
	switch (phase_) {
	case Enemy3::Phase::Approach:

		UpdateApproach();
		break;
	case Enemy3::Phase::Leave:
		UpdateLeave();
		break;
	}


	//�s��X�V
	Trans();
	camera_->Update();
	collider->Update();
	//�s��X�V��
	Object3d::Update();

}

//�]��
void Enemy3::Trans() {

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
void Enemy3::Fire() {
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
	XMFLOAT3 position = GetPosition();

	//�e�𐶐���������
	std::unique_ptr<EnemyBullet> newBullet;
	newBullet = EnemyBullet::Create(position, velocity, modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//�e��o�^
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

void Enemy3::Landing()
{
	//���R���C�_�[�̎擾
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
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
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	//position.z += callback.move.m128_f32[2];

	XMFLOAT3 eyepos = camera_->GetEye();
	XMFLOAT3 tarpos = camera_->GetTarget();

	eyepos.x += callback.move.m128_f32[0];

	tarpos.x += callback.move.m128_f32[0];

	//�R���C�_�[�X�V
	UpdateWorldMatrix();
	camera_->SetEye(eyepos);
	camera_->SetTarget(tarpos);
	collider->Update();

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0.0f,-1.0f,0.0f,0.0f };
	RaycastHit raycastHit;
	//�ڒn���
	if (onGround)
	{
		//�X���[�Y�ɍ�����邽�߂̋z������
		const float adsDistance = 0.2f;
		//�ڒn���ێ�
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//�s��X�V
			Object3d::Update();
		}
		//�n�ʂ������̂ŗ���
		else
		{
			onGround = false;
			fallVec = {};
		}
	}
	//�������
	else if (fallVec.y <= 0.0f)
	{
		if (colManager_->RayCast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * 2.0f))
		{
			//���n
			onGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			//�s��X�V
			Object3d::Update();
		}
	}

}

//�`��
void Enemy3::Draw() {

	//���f���̕`��
	Object3d::Draw();


}


//��ԕω��p�̍X�V�֐�
//�ڋ�
void Enemy3::UpdateApproach() {

	int randam = MyMath::RandomMTInt(0, 3);

	if (input_->TriggerKey(DIK_A) || input_->TriggerKey(DIK_D)) {
		switch (randam)
		{
		case 0:
			position.x = -25.0f;
			break;
		case 1:
			position.x = 0.0f;
			break;
		case 2:
			position.x = 25.0f;
			break;
		case 3:
			break;
		}
	}
	
	position.x += speed.x;
	position.y += speed.y;
	position.z += speed.z;

	//���˃^�C�}�[�J�E���g�_�E��
	fireTimer--;
	//�w�莞�ԂɒB����
	if (fireTimer <= 0) {
		//�e����
		Fire();
		//���˃^�C�}�[������
		fireTimer = MyMath::RandomMTInt(kFireInterval / 2, kFireInterval);
	}

	if (!onGround)
	{
		//�����������x
		const float fallAcc = -0.1f;
		const float fallVYMin = -2.0f;
		//����
		fallVec.y = max(fallVec.y + fallAcc, fallVYMin);
		//�ړ�
		position.x += fallVec.x;
		position.y += fallVec.y;
		position.z += fallVec.z;
	}

	//���񂾂�
	if (life_ <= 0) {
		isDead_ = true;
		life_ = 0;
	}
	if (position.z <= -200.0f)isDead_ = true;
}

//���E
void Enemy3::UpdateLeave() {
	//���x

}

//���[���h���W���擾
XMFLOAT3 Enemy3::GetWorldPosition() {

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}
void Enemy3::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_LANDSHAPE)return;
	else if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) return;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)life_--;
	}
	else if (attribute == COLLISION_ATTR_EARTH)
	{
		isDead_ = true;
	}

}