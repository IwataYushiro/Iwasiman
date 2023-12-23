#include "EnemyChase.h"
#include <cassert>
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GamePlayScene.h"

#include "MyMath.h"
using namespace DirectX;
using namespace IwasiEngine;
/*

*	EnemyCore.cpp

*	�ǂ������Ă���G
*/

EnemyChase::~EnemyChase()
{
}

std::unique_ptr<EnemyChase> EnemyChase::Create(const Model* model,
	const Player* player, GamePlayScene* gamescene, [[maybe_unused]] unsigned short level)
{
	std::unique_ptr<EnemyChase> ins = std::make_unique<EnemyChase>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	//���@�̃Z�b�g
	if (player)ins->SetPlayer(player);
	//�Q�[���V�[���̃Z�b�g
	if (gamescene)ins->SetGameScene(gamescene);
	return ins;
}

bool EnemyChase::Initialize()
{
	//������
	if (!Object3d::Initialize()) return false;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), this->radius_));
	//�G�����Ŕ\�͂͑����^
	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_ENEMY_DEATH);
	//�e��p�����[�^�ݒ�
	Parameter();
	//��
	particleFire_ = Particle::LoadFromParticleTexture("particle8.png");
	pmFire_ = ParticleManager::Create();
	pmFire_->SetBlendMode(ParticleManager::BP_SUBTRACT);
	pmFire_->SetParticleModel(particleFire_.get());
	return true;
}

void EnemyChase::Reset()
{
	Parameter();//�e��p�����[�^����
}

void EnemyChase::Parameter()
{
	//���C�t�����l
	const int32_t startLife = 5;
	life_ = startLife;
	//���]������
	isReverse_ = false;
	//���S�t���O
	isDead_ = false;

}

void EnemyChase::Update(const bool isStart)
{
	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pmFire_->SetCamera(camera_);
	//�펞�p�[�e�B�N��
	UpdateParticleSkin();

	if (!isStart)
	{
		//���x
		XMFLOAT3 velocity;

		const XMFLOAT3 velDefault = { 0.3f, 0.0f, 0.0f };//�ʏ펞�X�s�[�h
		const XMFLOAT3 velReverse = { -0.3f, 0.0f, 0.0f };//���]���X�s�[�h

		//�ړ�
		if (!isReverse_)velocity = velDefault;
		else velocity = velReverse;

		position_.x += velocity.x;
		position_.y += velocity.y;
		position_.z += velocity.z;

		//X�����w��̈ʒu�ɓ��B�����甽�]
		const float reversePosX = 100.0f;

		if (position_.x >= reversePosX) {
			//�����灩
			isReverse_ = true;
		}
		if (position_.x <= -reversePosX) {
			//�����灨
			isReverse_ = false;
		}
	}
	//���W��]��
	Trans();
	//�X�V
	camera_->Update();		//�J����
	Object3d::Update();		//3D�I�u�W�F�N�g
	collider_->Update();	//�R���C�_�[
	pmFire_->Update();		//�p�[�e�B�N��
}

void EnemyChase::Trans()
{
	//���[���h���W
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

const XMFLOAT3 EnemyChase::GetWorldPosition() const
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void EnemyChase::Draw()
{
	//���f���̕`��
	Object3d::Draw();
}

void EnemyChase::DrawParticle()
{
	pmFire_->Draw();
}

void EnemyChase::OnCollision([[maybe_unused]] const CollisionInfo& info, [[maybe_unused]] const unsigned short attribute, [[maybe_unused]] const unsigned short subAttribute)
{
}

void EnemyChase::UpdateParticleSkin()
{
	//���v���Z�b�g
	const ParticleManager::Preset fire =
	{
		particleFire_.get(),
		position_,
		{radius_*2.0f,radius_ * 2.0f,radius_ * 2.0f},
		{ -3.0f,0.1f,0.1f },
		{ 0.0f,0.001f,0.0f },
		6,
		{MyMath::RandomMTFloat(3.0f,10.0f), 0.0f },
		{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
		{ 0.0f,0.0f,0.0f,1.0f }

	};
	//�}�C�i�X���W�p
	struct MinusPreset
	{
		XMFLOAT3 minusStartPos;			//�������W
		XMFLOAT3 minusVel;			//���x
	};
	//���W�}�C�i�X�v���Z�b�g
	const MinusPreset minus =
	{
		{position_.x-radius_,position_.y,position_.z},
		{-fire.vel.x,-fire.vel.y,-fire.vel.z}
	};

	//�̂𕢂����G�t�F�N�g
	if (!isReverse_)//�ʏ펞
	{
		pmFire_->ActiveX(fire.particle, fire.startPos, fire.pos, fire.vel,
			fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
	}
	else//���]��
	{
		pmFire_->ActiveX(fire.particle, minus.minusStartPos, fire.pos, minus.minusVel,
			fire.acc, fire.num, fire.scale, fire.startColor, fire.endColor);
	}

}
