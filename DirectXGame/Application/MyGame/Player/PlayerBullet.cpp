#include "PlayerBullet.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;

/*

*	PlayerBullet.cpp

*	���@�̒e

*/

//�ÓI�����o�ϐ��̎���
CollisionManager* PlayerBullet::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<PlayerBullet> PlayerBullet::Create(const XMFLOAT3& position, const XMFLOAT3& velocity, Model* model)
{
	//�C���X�^���X����
	std::unique_ptr<PlayerBullet> ins = std::make_unique<PlayerBullet>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize(position, velocity))
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	return ins;
}

//������
bool PlayerBullet::Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity)
{
	if (!Object3d::Initialize()) return false;

	//�����Ŏ󂯎�����������W���Z�b�g
	SetPosition(position);
	velocity_ = velocity;

	//�R���C�_�[�ǉ�
	const XMVECTOR colliderOffset = { 0.0f,radius_,0.0f,0.0f };
	SetCollider(new SphereCollider(colliderOffset, radius_));
	collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);

	return true;
}

void PlayerBullet::Reset() { isDead_ = true; }
//�X�V
void PlayerBullet::Update() {
	//���W���ړ�������
	XMFLOAT3 pos = Object3d::GetPosition();

	pos.x += velocity_.x;
	pos.y += velocity_.y;
	pos.z += velocity_.z;

	Object3d::SetPosition(pos);
	//�s��X�V
	XMMATRIX world;

	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(Object3d::GetScale().x, Object3d::GetScale().y, Object3d::GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(Object3d::GetRotation().z)
		* XMMatrixRotationX(Object3d::GetRotation().x) * XMMatrixRotationY(Object3d::GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(Object3d::GetPosition().x,
		Object3d::GetPosition().y, Object3d::GetPosition().z);

	//����
	matWorld = matScale * matRot * matTrans;

	Object3d::SetWorld(matWorld);
	camera_->Update();
	Object3d::Update();
	//���Ԍo�߂Ŏ��S
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

//�`��
void PlayerBullet::Draw() {
	//���f���̕`��
	Object3d::Draw();
}

//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
void PlayerBullet::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute) {

	if (attribute == COLLISION_ATTR_LANDSHAPE)isDead_ = true;
	else if (attribute == COLLISION_ATTR_ENEMYS) 
	{
		if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
		else isDead_ = true;
	}
	
}
//���[���h���W���擾
XMFLOAT3 PlayerBullet::GetWorldPosition() {

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}