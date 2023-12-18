#include "Goal.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	Goal.cpp

*	�S�[��

*/
std::unique_ptr<Goal> Goal::Create(const Model* model)
{
	//�C���X�^���X����
	std::unique_ptr<Goal> ins = std::make_unique<Goal>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	return ins;
}

bool Goal::Initialize()
{
	//������
	if (!Object3d::Initialize()) return false;
	//�S�[��������
	isGoal_ = false;
	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	//�S�[���{��
	collider_->SetAttribute(COLLISION_ATTR_GOAL);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	return true;
	
}

void Goal::Reset() { isGoal_ = false;/*�S�[������������*/ }

void Goal::Update()
{
	//���[���h���W��]��
	Trans();
	//�X�V
	camera_->Update();	//�J����
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

const XMFLOAT3 Goal::GetWorldPosition()const
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void Goal::Draw()
{
	Object3d::Draw();//���f���̕`��
}

void Goal::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_PLAYERS)//���@�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) isGoal_ = true;	//���@�{�̂��G�ꂽ��S�[��
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;	//���@�̒e���ች���N����Ȃ�
	}
}
