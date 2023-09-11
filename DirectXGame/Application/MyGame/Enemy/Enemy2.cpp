#include "Enemy2.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;

CollisionManager* Enemy2::colManager_ = CollisionManager::GetInstance();

Enemy2::~Enemy2() {
}

std::unique_ptr<Enemy2> Enemy2::Create(Model* model, Model* bullet, Player* player, GamePlayScene* gamescene, int level)
{
	//�C���X�^���X����
	std::unique_ptr<Enemy2> ins = std::make_unique<Enemy2>();
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
bool Enemy2::Initialize() {

	if (!Object3d::Initialize()) return false;
	Parameter();

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	return true;
}

//�p�����[�^
void Enemy2::Parameter() {

	isReverse_ = false;
	//�W�����v������
	onGround = true;
	//�����t�F�[�Y
	phase_ = Phase::Approach;

	//���˃^�C�}�[������
	fireTimer = kFireInterval;

	life_ = 2;
	isDead_ = false;

	kFireInterval = MyMath::RandomMTInt(100, 150);
}

//���Z�b�g
void Enemy2::Reset() { Parameter(); }

//�X�V
void Enemy2::Update() {


	//���W���ړ�������
	switch (phase_) {
	case Enemy2::Phase::Approach:

		UpdateApproach();
		break;
	case Enemy2::Phase::Leave:
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
	XMFLOAT3 position = GetPosition();

	//�e�𐶐���������
	std::unique_ptr<EnemyBullet> newBullet;
	newBullet = EnemyBullet::Create(position, velocity, modelBullet_);
	newBullet->SetCamera(camera_);
	newBullet->Update();

	//�e��o�^
	gameScene_->AddEnemyBullet(std::move(newBullet));

}

void Enemy2::Landing()
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
void Enemy2::Draw() {

	//���f���̕`��
	Object3d::Draw();


}


//��ԕω��p�̍X�V�֐�
//�ڋ�
void Enemy2::UpdateApproach() {
	//���x
	XMFLOAT3 velocity;

	//�ړ�
	velocity = { 0.1f, 0.0f, -0.2f };
	if(isReverse_)position.x -= velocity.x;
	else position.x += velocity.x;

	position.y += velocity.y;
	position.z += velocity.z;

	if (position.x >= 25.0f) {
		isReverse_ = true;
	}
	if (position.x <= -25.0f) {
		isReverse_ = false;
	}
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
void Enemy2::UpdateLeave() {
	//���x

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
void Enemy2::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
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
