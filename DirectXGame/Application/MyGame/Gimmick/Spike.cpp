#include "Spike.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	Spike.cpp

*	������ƒɂ��g�Q

*/

std::unique_ptr<Spike> Spike::Create(const Model* model, const Player* player)
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
	//���@�̃Z�b�g
	if (player)ins->SetPlayer(player);
	return ins;
}

bool Spike::Initialize()
{
	//������
	if (!Object3d::Initialize()) return false;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR(), radius_));
	//�g�Q�̃M�~�b�N
	collider_->SetAttribute(COLLISION_ATTR_GIMMICK);
	collider_->SetSubAttribute(SUBCOLLISION_ATTR_GIMMICK_SPIKE);


	return true;

}

void Spike::Update()
{
	//���W��]��
	Trans();
	//�X�V
	camera_->Update();	//�J����
	Object3d::Update();	//3D�I�u�W�F�N�g
}

void Spike::Trans()
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
void Spike::Draw()
{
	Object3d::Draw();//�g�Q��`��
}

void Spike::OnCollision([[maybe_unused]] const CollisionInfo& info, [[maybe_unused]] const unsigned short attribute, [[maybe_unused]] const unsigned short subAttribute)
{
	
}