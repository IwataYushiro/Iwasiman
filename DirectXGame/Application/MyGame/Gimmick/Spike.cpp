#include "Spike.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;

/*

*	Spike.cpp

*	������ƒɂ��g�Q

*/

//�ÓI�����o�ϐ��̎���
CollisionManager* Spike::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<Spike> Spike::Create(Model* model, Player* player)
{
	//�C���X�^���X����
	std::unique_ptr<Spike> ins = std::make_unique<Spike>();

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

bool Spike::Initialize()
{
	if (!Object3d::Initialize()) return false;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	collider_->SetAttribute(COLLISION_ATTR_GIMMICK);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_GIMMICK_SPIKE);


	return true;

}

void Spike::Update()
{
	Trans();
	camera_->Update();
	Object3d::Update();
}

void Spike::Trans()
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

XMFLOAT3 Spike::GetWorldPosition()
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void Spike::Draw()
{
	Object3d::Draw();
}

void Spike::OnCollision([[maybe_unused]] const CollisionInfo& info, [[maybe_unused]] unsigned short attribute, [[maybe_unused]] unsigned short subAttribute)
{
	
}