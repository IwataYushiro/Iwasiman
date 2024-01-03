#include "Goal.h"
#include "SphereCollider.h"
#include "MyMath.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;
using namespace MyMath;
/*

*	Goal.cpp

*	�S�[��

*/
std::unique_ptr<Goal> Goal::Create(const Model* model, const bool isLandShape)
{
	//�C���X�^���X����
	std::unique_ptr<Goal> ins = std::make_unique<Goal>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize(isLandShape))
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	return ins;
}

bool Goal::Initialize(const bool isLandShape)
{
	//������
	if (!Object3d::Initialize()) return false;
	//�S�[��������
	isGoal_ = false;
	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	//�S�[���{��
	//�n�`���t���O���I���Ȃ�n�`��
	if(isLandShape)collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	else collider_->SetAttribute(COLLISION_ATTR_GOAL);//�I�t�Ȃ�S�[������
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	
	//�p�[�e�B�N��
	particle_ = Particle::LoadFromParticleTexture("particle1.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(particle_.get());

	return true;
	
}

void Goal::Reset() { isGoal_ = false;/*�S�[������������*/ }

void Goal::Update()
{
	//�p�[�e�B�N���}�l�[�W���[�ɃJ�������Z�b�g
	pm_->SetCamera(camera_);
	//�p�[�e�B�N���v���Z�b�g
	const ParticleManager::Preset goalEffect =
	{
		particle_.get(),
		position_,
		{ radius_*2.0f ,radius_ * 2.0f,radius_ * 2.0f },
		{ 0.1f,4.0f,0.1f },
		{ 0.0f,0.001f,0.0f },
		1,
		{3.0f, 0.0f },
		{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f},
		{RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),RandomMTFloat(0.0f,1.0f),1.0f}
	};
	//�S�[���̈ʒu��m�点��p�[�e�B�N��
	pm_->ActiveY(goalEffect.particle, goalEffect.startPos, goalEffect.pos, goalEffect.vel,
		goalEffect.acc, goalEffect.num, goalEffect.scale, goalEffect.startColor, goalEffect.endColor);

	//�S�[���͏펞����Ă���
	const float rotSpeedY = 1.0f;
	rotation_.y += rotSpeedY;
	//���[���h���W��]��
	Trans();
	//�X�V
	camera_->Update();	//�J����
	pm_->Update();		//�p�[�e�B�N��
	Object3d::Update();	//3D�I�u�W�F�N�g
}

void Goal::Trans()
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

void Goal::Draw()
{
	Object3d::Draw();//���f���̕`��
}

void Goal::DrawParticle()
{
	pm_->Draw();//�p�[�e�B�N���`��
}

void Goal::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_PLAYERS)//���@�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) isGoal_ = true;	//���@�{�̂��G�ꂽ��S�[��
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;	//���@�̒e���ች���N����Ȃ�
	}
}
