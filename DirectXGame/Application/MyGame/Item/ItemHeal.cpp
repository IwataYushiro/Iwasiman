#include "ItemHeal.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
CollisionManager* ItemHeal::colManager_ = CollisionManager::GetInstance();

ItemHeal::~ItemHeal()
{
	delete p_;
	delete pm_;
}

std::unique_ptr<ItemHeal> ItemHeal::Create(Model* model, Player* player)
{
	//�C���X�^���X����
	std::unique_ptr<ItemHeal> ins = std::make_unique<ItemHeal>();

	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	if (player)ins->SetPlayer(player);
	return ins;
}

bool ItemHeal::Initialize()
{
	if (!Object3d::Initialize()) return false;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider_->SetAttribute(COLLISION_ATTR_ITEM);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_NONE);


	return true;

}

void ItemHeal::Update()
{
	pm_->SetCamera(camera_);
	rotation_.y += 2.0f;

	Trans();
	camera_->Update();
	pm_->Update();
	Object3d::Update();
}

void ItemHeal::Trans()
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

XMFLOAT3 ItemHeal::GetWorldPosition()
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void ItemHeal::Draw()
{
	if (!isGet_)Object3d::Draw();
}

void ItemHeal::DrawParticle()
{
	pm_->Draw();
}

void ItemHeal::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (isGet_)return;//���d�q�b�g��h�~
	if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			pm_->ActiveY(p_, position_, { 8.0f ,8.0f,0.0f }, { 0.1f,4.0f,0.1f }, { 0.0f,0.001f,0.0f }, 30, { 2.0f, 0.0f });

			player_->SetLife(player_->GetLife() + 1);
			isGet_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;
	}
}
