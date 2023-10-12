#include "FallSphere.h"
#include "Player.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;

//�����鋅�Ə��鋅

//�ÓI�����o�ϐ��̎���
CollisionManager* FallSphere::colManager_ = CollisionManager::GetInstance();

std::unique_ptr<FallSphere> FallSphere::Create(Model* model, Player* player, unsigned short subAttribute)
{
	//�C���X�^���X����
	std::unique_ptr<FallSphere> ins = std::make_unique<FallSphere>();

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
	if (subAttribute)ins->collider_->SetSubAttribute(subAttribute);
	

	return ins;
}

bool FallSphere::Initialize()
{
	if (!Object3d::Initialize()) return false;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	
	return true;

}

void FallSphere::Update()
{
	
	if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE)UpdateFallSphereReturn();
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN)UpdateFallSphereReturn();
	else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_UPSPHERE)UpdateRiseSphere();

	Trans();
	camera_->Update();
	Object3d::Update();
}

void FallSphere::UpdateFallSphere()
{
	if (isRide_)
	{
		const float downSpeed = 0.1f;
		position_.y -= downSpeed;

		if (position_.y <= startPos_.y - 50.0f)
		{
			position_ = startPos_;
			isRide_ = false;
		}
	}
}

void FallSphere::UpdateFallSphereReturn()
{
	const float speed = 0.1f;
	
	if (isRide_)
	{
		if (player_->OnGround())position_.y -= speed;
		else
		{
			isRide_ = false;
			isReturn_ = true;
		}
	}
	else if (isReturn_)
	{
		position_.y += speed;
		if (position_.y >= startPos_.y)
		{
			position_ = startPos_;
			isReturn_ = false;
		}
	}
	
}

void FallSphere::UpdateRiseSphere()
{
	if (isRide_)
	{
		const float Speed = 0.1f;
		position_.y += Speed;

		if (position_.y >= startPos_.y + 50.0f)
		{
			position_ = startPos_;
			isRide_ = false;
		}
	}
}

void FallSphere::UpdateRiseSphereReturn()
{
	const float speed = 0.1f;

	if (isRide_)
	{
		if (player_->OnGround())position_.y += speed;
		else
		{
			isRide_ = false;
			isReturn_ = true;
		}
	}
	else if (isReturn_)
	{
		position_.y -= speed;
		if (position_.y <= startPos_.y)
		{
			position_ = startPos_;
			isReturn_ = false;
		}
	}
}

void FallSphere::Trans()
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

XMFLOAT3 FallSphere::GetWorldPosition()
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void FallSphere::Draw()
{
	Object3d::Draw();
}

void FallSphere::OnCollision([[maybe_unused]] const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (isRide_)return;

	if (attribute == COLLISION_ATTR_PLAYERS)
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			if (!isReturn_)startPos_ = position_;
			isRide_ = true;
			
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;

	}
}