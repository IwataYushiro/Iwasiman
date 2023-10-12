#include "Goal.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;

//�S�[��

//�ÓI�����o�ϐ��̎���
CollisionManager* Goal::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<Goal> Goal::Create(Model* model)
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
	if (!Object3d::Initialize()) return false;
	isGoal_ = false;
	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider_->SetAttribute(COLLISION_ATTR_GOAL);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);
	return true;
	
}

void Goal::Reset() { isGoal_ = false; }

void Goal::Update()
{
	Trans();
	camera_->Update();
	Object3d::Update();
}

void Goal::Trans()
{
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

XMFLOAT3 Goal::GetWorldPosition()
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
	Object3d::Draw();
}

void Goal::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE) isGoal_ = true;
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
		
	
}
