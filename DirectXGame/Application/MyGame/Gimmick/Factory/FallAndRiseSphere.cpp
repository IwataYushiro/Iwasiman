#include "FallAndRiseSphere.h"
#include "Player.h"
#include "MeshCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace IwasiEngine;
/*

*	FallAndRiseSphere.cpp

*	�����鋅�Ə��鋅

*/


std::unique_ptr<FallAndRiseSphere> FallAndRiseSphere::Create(Model* model, const Player* player, const unsigned short subAttribute)
{
	//�C���X�^���X����
	std::unique_ptr<FallAndRiseSphere> ins = std::make_unique<FallAndRiseSphere>();

	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize(model))
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	//���@���Z�b�g
	if (player)ins->SetPlayer(player);
	if(subAttribute)ins->collider_->SetSubAttribute(subAttribute);//�ǂ̃p�^�[�����͈����Ō��߂�

	return ins;
}

bool FallAndRiseSphere::Initialize(Model* model)
{
	//������
	if (!Object3d::Initialize()) return false;

	//�R���C�_�[�ǉ�
	SetCollider(new MeshCollider());
	//���b�V���R���C�_�[�ŐڐG�\�ȎO�p�`�̔z����\�z
	dynamic_cast<MeshCollider*>(collider_)->ConstructTriangles(model);
	//�n�`�̂ЂƂ�
	collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	return true;

}

void FallAndRiseSphere::Update()
{
	
	//�X�V��������
	if (!isSetStartPositionOnce_)
	{
		//�ŏ��̍��W���Z�b�g����
		startPos_ = position_;
		isSetStartPositionOnce_ = true;
	}

	//�����ɂ���Č��ʂ�ς���
	//�������@�����񂾂瓮�����Ȃ��悤�ɂ���
	if (!player_->IsBreak())
	{
		if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE)UpdateFallSphereReturn();				//�����鏰�̏ꍇ
		else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN)UpdateFallSphereReturn();	//�����Ė߂鏰�̏ꍇ
		else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_UPSPHERE)UpdateRiseSphere();					//���鏰�̏ꍇ
		else if (collider_->GetSubAttribute() == SUBCOLLISION_ATTR_GIMMICK_UPSPHERE_RETURN)UpdateRiseSphereReturn();	//�����Ė߂鋅�̏ꍇ
	}
	//���W�]��
	Trans();
	//�X�V
	camera_->Update();	//�J����
	Object3d::Update();	//3D�I�u�W�F�N�g

	
}

void FallAndRiseSphere::UpdateFallSphere()
{
	if (isRide_)//����Ă�ꍇ
	{
		//����������
		const float downSpeed = 0.1f;
		position_.y -= downSpeed;

		//Y���W�����̈ʒu�܂ŉ���������ŏ��̈ʒu�ɖ߂�
		if (position_.y <= startPos_.y - resetPosY_)
		{
			position_ = startPos_;
			isRide_ = false;
		}
	}
}

void FallAndRiseSphere::UpdateFallSphereReturn()
{
	const float speed = 0.1f;//���x

	if (isRide_)//����Ă�ꍇ
	{
		//����������
		if (player_->OnGround())position_.y -= speed;
		else//�n�ʂ��痣�ꂽ�ꍇ�߂�悤��
		{
			isReturn_ = true;
			isRide_ = false;
		}
	}
	else if (isReturn_)//���ꂽ�ꍇ
	{
		//���̈ʒu�܂Ŗ߂�悤�ɂ���
		position_.y += speed;
		if (position_.y >= startPos_.y)
		{
			position_ = startPos_;
			isReturn_ = false;
		}
	}
	
}

void FallAndRiseSphere::UpdateRiseSphere()
{
	if (isRide_)//����Ă�ꍇ
	{
		//���葱����
		const float Speed = 0.1f;
		position_.y += Speed;

		///Y���W�����̈ʒu�܂ŏ�������ŏ��̈ʒu�ɖ߂�
		if (position_.y >= startPos_.y + resetPosY_)
		{
			position_ = startPos_;
			isRide_ = false;
		}
	}
}

void FallAndRiseSphere::UpdateRiseSphereReturn()
{
	const float speed = 0.1f;//���x
	if (isRide_)//����Ă�ꍇ
	{
		//���葱����
		position_.y += speed;
		if (!player_->OnGround())//�n�ʂ��痣�ꂽ�ꍇ�߂�悤��
		{
			isReturn_ = true;
			isRide_ = false;
		}
	}
	else if (isReturn_)//���ꂽ�ꍇ
	{
		//���̈ʒu�܂Ŗ߂�悤�ɂ���
		if (!player_->OnGround())position_.y -= speed;
		if (position_.y <= startPos_.y)
		{
			position_ = startPos_;
			isReturn_ = false;
		}
	}
}

void FallAndRiseSphere::Draw()
{
	Object3d::Draw();//�`��
}

void FallAndRiseSphere::OnCollision([[maybe_unused]] const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)
{
	if (isRide_)return;//���d�q�b�g�̖h�~

	if (attribute == COLLISION_ATTR_PLAYERS)//���@�̏ꍇ
	{
		if (subAttribute == SUBCOLLISION_ATTR_NONE)//���@�{�̂̏ꍇ
		{
			//����ɏ���������ɂ���
			isRide_ = true;
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)return;//�e���������Ă����ɉ����N����Ȃ�

	}
}