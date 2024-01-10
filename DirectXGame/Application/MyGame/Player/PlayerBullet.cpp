#include "PlayerBullet.h"
#include <cassert>
#include "MyMath.h"
#include "SphereCollider.h"
#include "CollisionAttribute.h"

using namespace DirectX;
using namespace IwasiEngine;
using namespace MyMath;
/*

*	PlayerBullet.cpp

*	���@�̒e

*/

std::unique_ptr<PlayerBullet> PlayerBullet::Create(const XMFLOAT3& position, const XMFLOAT3& velocity,
	const Model* model, Particle* particle, ParticleManager* pm)
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
	//�e�����Ƃ��̉�
	if (particle)ins->particleFire_ = particle;
	if (pm)ins->pmFire_ = pm;
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
	//���@�̒e
	collider_->SetAttribute(COLLISION_ATTR_PLAYERS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_BULLET);

	return true;
}

void PlayerBullet::Reset() { isDead_ = true; }
//�X�V
void PlayerBullet::Update() {

	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pmFire_->SetCamera(camera_);
	//���W���ړ�������
	XMFLOAT3 pos = Object3d::GetPosition();

	pos.x += velocity_.x;
	pos.y += velocity_.y;
	pos.z += velocity_.z;

	
	//���W�̃Z�b�g
	Object3d::SetPosition(pos);
	//�s��]��
	Trans();
	
	UpdateParticle();//�p�[�e�B�N���X�V
	//�X�V
	camera_->Update();	//�J����
	Object3d::Update();	//3D�I�u�W�F�N�g
	pmFire_->Update();	//�p�[�e�B�N��
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

void PlayerBullet::DrawParticle()
{
	pmFire_->Draw();
}

//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
void PlayerBullet::OnCollision([[maybe_unused]] const CollisionInfo& info,const unsigned short attribute,const unsigned short subAttribute) {
	
	if (attribute == COLLISION_ATTR_LANDSHAPE)isDead_ = true;//�n�`�̏ꍇ�Ԃ����������
	else if (attribute == COLLISION_ATTR_ENEMYS) //�G�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;			//�G�̒e���Ɖ����N����Ȃ�
		else if (subAttribute == SUBCOLLISION_ATTR_ENEMY_ISDEAD)return;	//�G������ł��鈵���ł������N����Ȃ�
		else isDead_ = true;											//�G�{�̂͏���
	}
	
}

void PlayerBullet::UpdateParticle()
{
	//�p�[�e�B�N��
	const ParticleManager::Preset fire =	//���v���Z�b�g
	{
		particleFire_,
		position_,
		{ 0.05f ,2.0f,0.05f },
		{-velocity_.x,velocity_.y,velocity_.z},//�e�̑��x�Ɠ���
		{ 0.0f,0.001f,0.0f },
		3,
		{ 2.0f, 0.0f },
		{0.0f,MyMath::RandomMTFloat(0.0f,0.2f),MyMath::RandomMTFloat(0.9f,1.0f),1.0f },
		{0.0f,0.0f,0.0f,1.0f}
	};
	
	pmFire_->ActiveX(fire);
}
