#include "EnemyBullet.h"
#include <cassert>
#include "SphereCollider.h"

using namespace DirectX;

//������
void EnemyBullet::Initialize(const XMFLOAT3& position, const XMFLOAT3& velocity, Model* model) {

	Object3d::SetModel(model);
	
	//�����Ŏ󂯎�����������W���Z�b�g
	SetPosition(position);
	velocity_ = velocity;
	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,radius_,0.0f,0.0f }, radius_));

}

void EnemyBullet::Reset() { isDead_ = true; }

//�X�V
void EnemyBullet::Update() {
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
void EnemyBullet::Draw() {
	//���f���`��
	Object3d::Draw();
}

//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
void EnemyBullet::OnCollision(const CollisionInfo& info) { isDead_ = true; }

//���[���h���W���擾
XMFLOAT3 EnemyBullet::GetWorldPosition() {

	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}