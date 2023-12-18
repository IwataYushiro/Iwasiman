#include "EnemyBullet.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;

/*

*	EnemyBullet.cpp

*	�G�̒e

*/

std::unique_ptr<EnemyBullet> EnemyBullet::Create(const XMFLOAT3& position, const XMFLOAT3& velocity, const Model* model)
{
	//�C���X�^���X����
	std::unique_ptr<EnemyBullet> ins = std::make_unique<EnemyBullet>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize(position,velocity))
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	return ins;
}

//������
bool EnemyBullet::Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity) {
	if (!Object3d::Initialize()) return false;

	//�����Ŏ󂯎�����������W���Z�b�g
	SetPosition(position);
	velocity_ = velocity;

	//�R���C�_�[�ǉ�
	const XMVECTOR colliderOffset = { 0.0f,this->radius_,0.0f,0.0f };
	SetCollider(new SphereCollider(colliderOffset, this->radius_));
	//�G�̒e
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);

	return true;
}

void EnemyBullet::Reset() { isDead_ = true; }

//�X�V
void EnemyBullet::Update() {
	//���W���ړ�������
	XMFLOAT3 pos = Object3d::GetPosition();

	pos.x += velocity_.x;
	pos.y += velocity_.y;
	pos.z += velocity_.z;
	//���W�̃Z�b�g
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
	//�X�V
	camera_->Update();	//�J����
	Object3d::Update();	//3D�I�u�W�F�N�g
	
	//���Ԍo�߂Ŏ��S
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

//�`��
void EnemyBullet::Draw() {
	//���f���̕`��
	Object3d::Draw();
}

//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
void EnemyBullet::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute) {
	
	if (attribute == COLLISION_ATTR_PLAYERS)//���@�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)isDead_ = true;	//���@�{�̂ɓ������������
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;	//���@�̒e���Ɖ����N����Ȃ�
	}
}

//���[���h���W���擾
const XMFLOAT3 EnemyBullet::GetWorldPosition() const{

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}